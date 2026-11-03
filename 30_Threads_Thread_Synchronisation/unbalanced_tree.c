// -----------------------------------------------------------------------------
// Implements unbalanced tree for Exercise 30-2
// -----------------------------------------------------------------------------

// #define VERBOSE

#include <errno.h>
#include <error.h>
#include <pthread.h>
#ifdef VERBOSE
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "unbalanced_tree.h"

// -----------------------------------------------------------------------------
// Support functions
// -----------------------------------------------------------------------------

#define ERR_MSG_FMT "%s (line: %d)"

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
    rc = pthread_mutex_init(&(node -> node_mutex), &mtx_attr);
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
    int key_cmp, rc, line_num;
    tree_ptr_t parent, child, node;
    int result;

    if (root == NULL || key == NULL || strlen(key) == 0 || value == NULL || strlen(value) == 0) {
        return EINVAL;
    }

    for (parent = NULL, child = root; ;) {

        #ifdef VERBOSE
        printf("utree_add: root=%p, parent=%p, child=%p\n", (void *)root, (void *)parent, (void *)child);
        #endif

        line_num = __LINE__ + 1;
        rc = pthread_mutex_lock(&(child -> node_mutex));
        if (rc != 0) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }

        #ifdef VERBOSE
        printf("utree_add: locked CHILD MUTEX at %p\n", (void *)&(child -> node_mutex));
        #endif

        if (parent != NULL) {
            line_num = __LINE__ + 1;
            rc = pthread_mutex_unlock(&(parent -> node_mutex));
            if (rc != 0) {
                error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
            }

            #ifdef VERBOSE
            printf("utree_add: released PARENT MUTEX at %p\n", (void *)&(parent -> node_mutex));
            #endif

        }

        if (child -> key == NULL) {

            #ifdef VERBOSE
            printf("utree_add: NULL key found for child=%p\n", (void *)child);
            #endif

            int len = strlen(key);
            line_num = __LINE__ + 1;
            child -> key = (char *)malloc(len + 1);
            if (child -> key == NULL) {
                error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
            }
            strcpy(child -> key, key);
            len = strlen(value);
            if (child -> value != NULL) {

                #ifdef VERBOSE
                printf("utree_add: Non-NULL value found for NULL key in child=%p\n", (void *)child);
                #endif

                free(child -> value);
            }
            line_num = __LINE__ + 1;
            child -> value = (char *)malloc(len + 1);
            if (child -> value == NULL) {
                error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
            }
            strcpy(child -> value, value);
            result = 0;
            break;
        }

        key_cmp = strcmp(child -> key, key);
        if (key_cmp == 0) {
            result = EEXIST;
            break;
        }

        if (key_cmp < 0) {
            if (child -> left == NULL) {

                #ifdef VERBOSE
                printf("utree_add: adding new leaf on the left of child=%p\n", (void *)child);
                #endif

                line_num = __LINE__ + 1;
                tree_ptr_t node = (tree_ptr_t)malloc(sizeof(tree_t));
                if (node == NULL) {
                    error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
                }
                int rc = _init_node(node);
                if (rc != 0) {
                    error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
                }
                int len = strlen(key);
                line_num = __LINE__ + 1;
                node -> key = (char *)malloc(len + 1);
                if (node -> key == NULL) {
                    error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
                }
                strcpy(node -> key, key);
                len = strlen(value);
                line_num = __LINE__ + 1;
                node -> value = (char *)malloc(len + 1);
                if (node -> value == NULL) {
                    error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
                }
                strcpy(node -> value, value);
                child -> left = node;
                result = 0;
                break;
            } else {
                node   = child;
                parent = node;
                child  = node -> left;

                #ifdef VERBOSE
                printf("utree_add: descending tree on left branch with new parent=%p, new child=%p\n", (void *)parent, (void *)child);
                #endif

            }
        } else {
            if (child -> right == NULL) {

                #ifdef VERBOSE
                printf("utree_add: adding new leaf on the right of child=%p\n", (void *)child);
                #endif
                
                line_num = __LINE__ + 1;
                tree_ptr_t node = (tree_ptr_t)malloc(sizeof(tree_t));
                if (node == NULL) {
                    error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
                }
                int rc = _init_node(node);
                if (rc != 0) {
                    error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
                }
                int len = strlen(key);
                line_num = __LINE__ + 1;
                node -> key = (char *)malloc(len + 1);
                if (node -> key == NULL) {
                    error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
                }
                strcpy(node -> key, key);
                len = strlen(value);
                line_num = __LINE__ + 1;
                node -> value = (char *)malloc(len + 1);
                if (node -> value == NULL) {
                    error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
                }
                strcpy(node -> value, value);
                child -> right = node;
                result = 0;
                break;
            } else {
                node   = child;
                parent = node;
                child  = node -> right;

                #ifdef VERBOSE
                printf("utree_add: descending tree on right branch with new parent=%p, new child=%p\n", (void *)parent, (void *)child);
                #endif
                
            }
        }
    }

    line_num = __LINE__ + 1;
    rc = pthread_mutex_unlock(&(child -> node_mutex));
    if (rc != 0) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }

    #ifdef VERBOSE
    printf("utree_add: released CHILD MUTEX at %p\n", (void *)&(child -> node_mutex));
    #endif

    return result;
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

    if (root == NULL || key == NULL || strlen(key) == 0 || value == NULL) {
        if (value != NULL)
           *value = NULL;
        return FALSE;
    }

    for (parent = NULL, child = root; ;) {
        line_num = __LINE__ + 1;
        rc = pthread_mutex_lock(&(child -> node_mutex));
        if (rc != 0) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }

        #ifdef VERBOSE
        printf("utree_lookup: locked CHILD MUTEX at %p\n", (void *)&(child -> node_mutex));
        #endif

        if (parent != NULL) {
            line_num = __LINE__ + 1;
            rc = pthread_mutex_unlock(&(parent -> node_mutex));
            if (rc != 0) {
                error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
            }

            #ifdef VERBOSE
            printf("utree_lookup: released PARENT MUTEX at %p\n", (void *)&(parent -> node_mutex));
            #endif

        }

        if (child -> key == NULL) {
            if (value != NULL)
                *value = NULL;
            result = FALSE;
            break;
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
                *value = NULL;
                result = FALSE;
                break;
            } else {
                node   = child;
                parent = node;
                child  = node -> left;
            }
        } else {
            if (child -> right == NULL) {
                *value = NULL;
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
    rc = pthread_mutex_unlock(&(child -> node_mutex));
    if (rc != 0) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }

    #ifdef VERBOSE
    printf("utree_lookup: released CHILD MUTEX at %p\n", (void *)&(child -> node_mutex));
    #endif

    return result;
}
