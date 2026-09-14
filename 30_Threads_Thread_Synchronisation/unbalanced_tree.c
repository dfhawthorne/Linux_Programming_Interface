// -----------------------------------------------------------------------------
// Implements unbalanced tree for Exercise 30-2
// -----------------------------------------------------------------------------

#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>
#include "unbalanced_tree.h"

// -----------------------------------------------------------------------------
// Support functions
// -----------------------------------------------------------------------------

#define ERR_MSG_FMT ": %s (line: %d)\n"

int _init_node(tree_ptr_t node) {
    int rc, line_num;
    pthread_mutexattr_t mtx_attr;

    if (node == NULL) return EINVAL;

    node -> key   = NULL;
    node -> value = NULL;
    node -> left  = NULL;
    node -> right = NULL;

    line_num = __LINE__ + 1;
    rc = pthread_mutexattr_init(&mtx_attr);
    if (rc != 0) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }
    line_num = __LINE__ + 1;
    rc = pthread_mutexattr_settype(&mtx_attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    if (rc != 0) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }
    line_num = __LINE__ + 1;
    rc = pthread_mutex_init(&node -> node_mutex, &mtx_attr);
    if (rc != 0) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }
    line_num = __LINE__ + 1;
    rc = pthread_mutexattr_destroy(&mtx_attr);
    if (rc != 0) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }

    return 0;
}

// -----------------------------------------------------------------------------
// Implementations for interface functions
// -----------------------------------------------------------------------------

int utree_initialise(tree_ptr_t root) {
    return _init_node(root);
}

int utree_add(tree_ptr_t root, char *key, void *value) {

    if (root == NULL || key == NULL || value == NULL || strlen(key) == 0)
        return EINVAL;

    return ENOSYS;
}

int utree_delete(tree_ptr_t root, char *key) {

    if (root == NULL || key == NULL || strlen(key) == 0)
        return EINVAL;

    return ENOSYS;
}

Boolean utree_lookup(tree_ptr_t root, char *key, void **value) {
    int key_cmp, rc, line_num;
    tree_ptr_t parent, child, node;
    Boolean result;

    if (root == NULL || key == NULL || strlen(key) == 0) {
        value = NULL;
        return FALSE;
    }

    for (parent = NULL, child = root; ;) {
        line_num = __LINE__ + 1;
        rc = pthread_mutex_lock(&child -> node_mutex);
        if (rc != 0) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }
        if (parent != NULL) {
            line_num = __LINE__ + 1;
            rc = pthread_mutex_unlock(&parent -> node_mutex);
            if (rc != 0) {
                error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
            }
        }
        key_cmp = strcmp(child -> key, key);
        if (key_cmp == 0) {
            if (child -> value != NULL) {
                int value_len = strlen(child -> value);
                line_num = __LINE__ + 1;
                void *found_value = malloc(value_len+1);
                if (found_value == NULL) {
                    error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
                }
                strcpy((char*)found_value, (char *)child -> value);
                *value = found_value;
            } else {
                *value = NULL;
            }
            result = TRUE;
            break;
        }

        if (key_cmp < 0) {
            if (child -> left == NULL) {
                value = NULL;
                result = FALSE;
                break;
            } else {
                node   = child;
                parent = node;
                child  = node -> left;
            }
        } else {
            if (child -> right == NULL) {
                value = NULL;
                result = FALSE;
                break;
            } else {
                node   = child;
                parent = node;
                child  = node -> right;
            }
        }
    }

    line_num = __LINE__ + 1;
    rc = pthread_mutex_unlock(&child -> node_mutex);
    if (rc != 0) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }
    if (parent != NULL) {
        line_num = __LINE__ + 1;
        rc = pthread_mutex_unlock(&parent -> node_mutex);
        if (rc != 0) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }
    }

    return result;
}
