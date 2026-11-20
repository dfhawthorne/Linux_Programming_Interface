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

#pragma region Support functions

// -----------------------------------------------------------------------------
// Support functions
// -----------------------------------------------------------------------------

#define ERR_MSG_FMT "%s (line: %d)"

// -----------------------------------------------------------------------------
// Initialises a tree node with NULL pointers and a MUTEX.
//    Assumes that the storage for the node has already been allocated.
// Any failure in MUTEX manipulation results in a program crash.
// -----------------------------------------------------------------------------

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
    if (rc) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }
    line_num = __LINE__ + 1;
    rc = pthread_mutexattr_settype(&mtx_attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    if (rc) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }
    line_num = __LINE__ + 1;
    rc = pthread_mutex_init(&(node -> node_mutex), &mtx_attr);
    if (rc) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }
    line_num = __LINE__ + 1;
    rc = pthread_mutexattr_destroy(&mtx_attr);
    if (rc) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }

    return 0;
}

// -----------------------------------------------------------------------------
// Creates a tree node with a key-value pair.
//   NULL parameters result in a program crash.
//   Any failure in memory allocation results in a program crash.
// -----------------------------------------------------------------------------

tree_ptr_t _new_node(char *key, char *value) {
	int line_num;

    line_num = __LINE__ + 1;
    if (!key || !value)
		error(EXIT_FAILURE, EINVAL, ERR_MSG_FMT, __FILE__, line_num);
                
	line_num = __LINE__ + 1;
	tree_ptr_t node = (tree_ptr_t)malloc(sizeof(tree_t));
	if (!node)
		error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);

    line_num = __LINE__ + 1;
	int rc = _init_node(node);
	if (rc)
		error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

	int len = strlen(key);
	line_num = __LINE__ + 1;
	node -> key = (char *)malloc(len + 1);
	if (!(node -> key))
		error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
	strcpy(node -> key, key);

	len = strlen(value);
	line_num = __LINE__ + 1;
	node -> value = (char *)malloc(len + 1);
	if (!(node -> value))
		error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
	strcpy(node -> value, value);
	
	return node;
}

// -----------------------------------------------------------------------------
// Frees the storage associated with the key-value pair for a tree node and
// unlocks the MUTEX.
//   With the storage released for the key-value pair, the MUTEX becomes moot.
//   The reason that _destroy_node() functionality is split across two
//   functions is the root node is treated differently to other nodes.
// -----------------------------------------------------------------------------

int _free_node(tree_ptr_t node) {
    int rc, line_num;

    if (!node) return EINVAL;

    free(node -> key);
    free(node -> value);

    #ifdef VERBOSE
    printf(
        "_free_node: freed key at %p, value at %p\n",
        (void *)(node -> key),
        (void *)(node -> value)
    );
    #endif

    line_num = __LINE__ + 1;
    rc = pthread_mutex_unlock(&(node -> node_mutex));
    if (rc) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }

    #ifdef VERBOSE
    printf(
        "_free_node: released MUTEX at %p\n",
        (void *)&(node -> node_mutex)
    );
    #endif

    return 0;
}

// -----------------------------------------------------------------------------
// Destroys the storage associated with a tree node and destroys the MUTEX.
//   The _free_node() utility is used.
// -----------------------------------------------------------------------------

int _destroy_node(tree_ptr_t node) {
    int rc, line_num;

    if (!node) return EINVAL;

    rc = _free_node(node);
    if (rc) return rc;

    line_num = __LINE__ + 1;
    rc = pthread_mutex_destroy(&(node -> node_mutex));
    if (rc)
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

    #ifdef VERBOSE
    printf(
        "_destroy_node: destroyed MUTEX at %p\n",
        (void *)&(node -> node_mutex)
    );
    #endif

    #ifdef VERBOSE
    printf(
        "_destroy_node: freed node at %p\n",
        (void *)node
    );
    #endif
    free(node);

    return 0;
}

// -----------------------------------------------------------------------------
// Moves the key-value pair from one tree node to another.
//   This function is used to promote another node into the root.
// -----------------------------------------------------------------------------

int _move_node(tree_ptr_t dest, tree_ptr_t src) {
    int rc, line_num;

    if (!dest || !src) return EINVAL;

    #ifdef VERBOSE
    printf(
        "_move_node: from %p (key='%s') to %p\n",
        (void *)src,
        src -> key,
        (void *)dest
    );
    #endif

    if (dest -> key)   free(dest -> key);
    if (dest -> value) free(dest -> value);

    dest -> key   = src -> key;
    dest -> value = src -> value;
    dest -> left  = src -> left;
    dest -> right = src -> right;

    #ifdef VERBOSE
    printf(
        "_move_node: new node at %p, key='%s', value='%s', left=%p ('%s'), right=%p ('%s')\n",
        (void *)dest,
        dest -> key ? dest -> key : "",
        (char *)(dest -> value ? dest -> value : ""),
        (void *)dest -> left,
        dest -> left ? dest -> left -> key : "",
        (void *)dest -> right,
        dest -> right ? dest -> right -> key : ""
    );
    #endif

    line_num = __LINE__ + 1;
    rc = pthread_mutex_unlock(&(src -> node_mutex));
    if (rc)
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

    #ifdef VERBOSE
    printf(
        "_move_node: released MUTEX at %p ('%s')\n",
        (void *)&(src -> node_mutex),
        src -> key ? src -> key : ""
    );
    #endif

    line_num = __LINE__ + 1;
    rc = pthread_mutex_destroy(&(src -> node_mutex));
    if (rc)
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

    #ifdef VERBOSE
    printf(
        "_move_node: destroyed MUTEX at %p ('%s')\n",
        (void *)&(src -> node_mutex),
        src -> key ? src -> key : ""
    );
    printf(
        "_move_node: free node at %p ('%s')\n",
        (void *)src,
        src -> key ? src -> key : ""
    );
    #endif

    free(src);

    return 0;
}

#pragma endregion

// -----------------------------------------------------------------------------
// Implementations for interface functions
// -----------------------------------------------------------------------------

#pragma region utree_initialise

// -----------------------------------------------------------------------------
// Initialises a tree by formatting a pre-allocated tree node as the root.
// -----------------------------------------------------------------------------

int utree_initialise(tree_ptr_t root) {
    return _init_node(root);
}

#pragma endregion

#pragma region utree_add

// -----------------------------------------------------------------------------
// Adds a key-value pair to a tree with left branch having a key less than
// parent's.
//   Any failure to lock the MUTEX or to allocate storage results in a program
// crash.
// -----------------------------------------------------------------------------

int utree_add(tree_ptr_t root, char *key, void *value) {
    int key_cmp, rc, line_num;
    tree_ptr_t parent, child;
    int result;

    if (root == NULL || key == NULL || strlen(key) == 0 || value == NULL || strlen(value) == 0) {
        return EINVAL;
    }

    line_num = __LINE__ + 1;
    rc = pthread_mutex_lock(&(root -> node_mutex));
    if (rc) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }

    #ifdef VERBOSE
    printf("utree_add: locked ROOT MUTEX at %p\n", (void *)&(root -> node_mutex));
    #endif

    if (root -> key == NULL) {

        #ifdef VERBOSE
        printf("utree_add: NULL key found for root=%p\n", (void *)root);
        #endif

        int len = strlen(key);
        line_num = __LINE__ + 1;
        root -> key = (char *)malloc(len + 1);
        if (root -> key == NULL) {
            error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
        }
        strcpy(root -> key, key);

        len = strlen(value);
        if (root -> value != NULL) {

            #ifdef VERBOSE
            printf("utree_add: Non-NULL value found for NULL key in root=%p\n", (void *)root);
            #endif

            free(root -> value);
        }
        line_num = __LINE__ + 1;
        root -> value = (char *)malloc(len + 1);
        if (root -> value == NULL) {
            error(EXIT_FAILURE, ENOMEM, ERR_MSG_FMT, __FILE__, line_num);
        }
        strcpy(root -> value, value);
        
        line_num = __LINE__ + 1;
        rc = pthread_mutex_unlock(&(root -> node_mutex));
        if (rc) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }

        #ifdef VERBOSE
        printf("utree_add: released ROOT MUTEX at %p\n", (void *)&(root -> node_mutex));
        #endif

        return 0;
    }

    for (parent = NULL, child = root; ;) {

        #ifdef VERBOSE
        printf("utree_add: root=%p, parent=%p, child=%p\n", (void *)root, (void *)parent, (void *)child);
        #endif

        line_num = __LINE__ + 1;
        if (child -> key == NULL) {
            error(EXIT_FAILURE, EINVAL, ERR_MSG_FMT, __FILE__, line_num);
        }

        key_cmp = strcmp(key, child -> key);
        if (key_cmp == 0) {
            result = EEXIST;
            break;
        }

        if (key_cmp < 0) {
            if (child -> left == NULL) {

                #ifdef VERBOSE
                printf("utree_add: adding new leaf on the left of child=%p\n", (void *)child);
                #endif

                tree_ptr_t node = _new_node(key, value);
                child -> left = node;
                result = 0;
                break;
            } else {
                tree_ptr_t node   = child;
                parent = node;
                child  = node -> left;

                #ifdef VERBOSE
                printf(
                    "utree_add: descending tree on left branch with new parent=%p, new child=%p\n",
                    (void *)parent,
                    (void *)child
                );
                #endif

            }
        } else {
            if (child -> right == NULL) {

                #ifdef VERBOSE
                printf("utree_add: adding new leaf on the right of child=%p\n", (void *)child);
                #endif
                
                tree_ptr_t node = _new_node(key, value);
                child -> right = node;
                result = 0;
                break;
            } else {
                tree_ptr_t node   = child;
                parent = node;
                child  = node -> right;

                #ifdef VERBOSE
                printf(
                    "utree_add: descending tree on right branch with new parent=%p, new child=%p\n",
                    (void *)parent,
                    (void *)child
                );
                #endif
                
            }
        }

        line_num = __LINE__ + 1;
        rc = pthread_mutex_lock(&(child -> node_mutex));
        if (rc) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }

        #ifdef VERBOSE
        printf("utree_add: locked CHILD MUTEX at %p\n", (void *)&(child -> node_mutex));
        #endif

        line_num = __LINE__ + 1;
        rc = pthread_mutex_unlock(&(parent -> node_mutex));
        if (rc) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }

        #ifdef VERBOSE
        printf("utree_add: released PARENT MUTEX at %p\n", (void *)&(parent -> node_mutex));
        #endif

    }

    line_num = __LINE__ + 1;
    rc = pthread_mutex_unlock(&(child -> node_mutex));
    if (rc) {
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
    }

    #ifdef VERBOSE
    printf("utree_add: released CHILD MUTEX at %p\n", (void *)&(child -> node_mutex));
    #endif

    return result;
}

#pragma endregion

#pragma region utree_delete

// -----------------------------------------------------------------------------
// utree_delete:
//   Search for a node with the supplied key. If the key is not found, the
// function succeeds because the key does not exist in the tree.
// -----------------------------------------------------------------------------

int utree_delete(tree_ptr_t root, char *key) {
    int rc, line_num, result;
    tree_ptr_t parent, child;

    if (root == NULL || key == NULL || strlen(key) == 0)
        return EINVAL;

    #ifdef VERBOSE
    printf(
        "utree_delete: root=%p, key='%s'\n",
        (void *)root,
        key
    );
    #endif

    line_num = __LINE__ + 1;
    rc = pthread_mutex_lock(&(root -> node_mutex));
    if (rc)
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

    #ifdef VERBOSE
    printf(
        "utree_delete: locked ROOT MUTEX at %p\n",
        (void *)&(root -> node_mutex)
    );
    #endif

    if (!(root -> key)) {
        line_num = __LINE__ + 1;
        rc = pthread_mutex_unlock(&(root -> node_mutex));
        if (rc)
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

        #ifdef VERBOSE
        printf(
            "utree_delete: released ROOT MUTEX at %p\n",
            (void *)&(root -> node_mutex)
        );
        printf(
            "utree_delete: '%s' deleted\n",
            key
        );
        #endif

        return 0;
    }

    result = 0;

    for (parent = NULL, child = root; ;) {
        line_num = __LINE__ + 1;
        if (!(child -> key))
            error(EXIT_FAILURE, EINVAL, ERR_MSG_FMT, __FILE__, line_num);

        #ifdef VERBOSE
        printf(
            "utree_delete: parent=%p ('%s'), child=%p ('%s'), left=%p, right=%p\n",
            (void *)parent,
            parent ? parent -> key : "",
            (void *)child,
            child ? child -> key : "",
            child ? (void *)(child -> left) : NULL,
            child ? (void *)(child -> right) : NULL
        );
        #endif

        int key_cmp = strcmp(key, child -> key);
        if (!key_cmp) {

            #ifdef VERBOSE
            printf(
                "utree_delete: key ('%s') found at %p\n",
                key,
                (void *)child
            );
            #endif

            tree_ptr_t old_left_branch = child -> left;
            tree_ptr_t old_right_branch = child -> right;
            tree_ptr_t new_left_branch = old_right_branch ?
                old_right_branch -> left : NULL;

            #ifdef VERBOSE
            printf(
                "utree_delete: child=%p ('%s'), old_left_branch=%p ('%s'), old_right_branch=%p ('%s'), new_left_branch=%p ('%s')\n",
                (void *)child,
                child ? child->key : "",
                (void *)old_left_branch,
                old_left_branch ? old_left_branch->key : "",
                (void *)old_right_branch,
                old_right_branch ? old_right_branch->key : "",
                (void *)new_left_branch,
                new_left_branch ? new_left_branch->key : ""
            );
            #endif

            if (!old_left_branch) {
                if (!old_right_branch) {   // Leaf
                    if (!parent) {       // Leaf is the root
                        result = _free_node(child);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: cleared ROOT leaf node at %p\n",
                            (void *)child
                        );
                        printf(
                            "utree_delete: '%s' deleted\n",
                            key
                        );
                        #endif

                        return result;

                    } else {
                        result = _destroy_node(child);

                        if (parent -> left == child) {
                            parent -> left = NULL;

                            #ifdef VERBOSE
                            printf(
                                "utree_delete: deleted leaf node at %p on left branch of %p\n",
                                (void *)child,
                                (void *)parent
                            );
                            #endif

                        } else {
                            parent -> right = NULL;

                            #ifdef VERBOSE
                            printf(
                                "utree_delete: deleted leaf node at %p on right branch of %p\n",
                                (void *)child,
                                (void *)parent
                            );
                            #endif

                        }
                    }
                } else {                        // promote right branch
                    line_num = __LINE__ + 1;
                    rc = pthread_mutex_lock(&(old_right_branch -> node_mutex));
                    if (rc)
                        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                    #ifdef VERBOSE
                    printf(
                        "utree_delete: locked right branch of CHILD MUTEX at %p\n",
                        (void *)&(old_right_branch -> node_mutex)
                    );
                    #endif

                    if (!parent) {

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: about to move %p to root at %p\n",
                            (void *)(root -> right),
                            (void *)root
                        );
                        #endif
                        
                        result = _move_node(root, root -> right);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: '%s' deleted\n",
                            key
                        );
                        #endif

                        line_num = __LINE__ + 1;
                        rc = pthread_mutex_unlock(&(root -> node_mutex));
                        if (rc)
                            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: released root MUTEX %p, key='%s'\n",
                            (void *)root,
                            root -> key ? root -> key : ""
                        );
                        #endif

                        return result;
                    } else {
                        result = _destroy_node(child);

                        if (parent -> left == child) {
                            parent -> left = old_right_branch;
                        } else {
                            parent -> right = old_right_branch;
                        }

                        line_num = __LINE__ + 1;
                        rc = pthread_mutex_unlock(&(old_right_branch -> node_mutex));
                        if (rc)
                            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: released right branch of CHILD MUTEX at %p\n",
                            (void *)&(old_right_branch -> node_mutex)
                        );
                        #endif

                    }
                }
            } else {

                #ifdef VERBOSE
                printf(
                    "utree_delete: child (%p) has a left branch (%p)\n",
                    (void *)child,
                    (void *)(child -> left)
                );
                #endif

                if (!old_right_branch) {   // promote left branch
                    line_num = __LINE__ + 1;
                    rc = pthread_mutex_lock(&(child -> left -> node_mutex));
                    if (rc)
                        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                    #ifdef VERBOSE
                    printf(
                        "utree_delete: locked left branch of CHILD MUTEX at %p\n",
                        (void *)&(child -> left -> node_mutex)
                    );
                    #endif

                    if (!parent) {
                        result = _move_node(root, root -> left);
                        parent = root;

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: moved left branch of child at %p to root at %p\n",
                            child ? (void *)&(child -> left) : NULL,
                            (void *)root
                        );
                        #endif

                    } else {
                        result = _destroy_node(child);

                        if (parent -> left == child) {
                            parent -> left = child -> left;
                        } else {
                            parent -> right = child -> left;
                        }

                        line_num = __LINE__ + 1;
                        rc = pthread_mutex_unlock(&(child -> left -> node_mutex));
                        if (rc)
                            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: released left branch of CHILD MUTEX at %p\n",
                            (void *)&(child -> left -> node_mutex)
                        );
                        #endif

                    }
                } else {
                    
                    #ifdef VERBOSE
                    printf(
                        "utree_delete: child (%p) has a right branch (%p)\n",
                        (void *)child,
                        (void *)(child -> right)
                    );
                    #endif

                    // promote right branch and attached old left branch at tail of new left branch

                    #ifdef VERBOSE
                    printf("utree_delete: promote right branch and attached old left branch at tail of new left branch\n");
                    printf(
                        "utree_delete: root=%p ('%s'), parent=%p ('%s'), child=%p ('%s'), old_left_branch=%p ('%s'), old_right_branch=%p ('%s'), new_left_branch=%p ('%s')\n",
                        (void *)root,
                        root ? root->key : "",
                        (void *)parent,
                        parent ? parent->key : "",
                        (void *)child,
                        child ? child->key : "",
                        (void *)old_left_branch,
                        old_left_branch ? old_left_branch->key : "",
                        (void *)old_right_branch,
                        old_right_branch ? old_right_branch->key : "",
                        (void *)new_left_branch,
                        new_left_branch ? new_left_branch->key : ""
                    );
                    #endif

                    line_num = __LINE__ + 1;
                    rc = pthread_mutex_lock(&(old_left_branch -> node_mutex));
                    if (rc)
                        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                    #ifdef VERBOSE
                    printf(
                        "utree_delete: locked Left grandchild MUTEX at %p ('%s')\n",
                        (void *)&(old_left_branch -> node_mutex),
                        old_left_branch -> key
                    );
                    #endif

                    line_num = __LINE__ + 1;
                    rc = pthread_mutex_lock(&(old_right_branch -> node_mutex));
                    if (rc)
                        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                    #ifdef VERBOSE
                    printf(
                        "utree_delete: locked Right grandchild MUTEX at %p ('%s')\n",
                        (void *)&(old_right_branch -> node_mutex),
                        old_right_branch->key
                        );
                    #endif
                
                    if (!parent) {

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: about to move node from %p ('%s') to root at %p ('%s') \n",
                            (void *)child -> right,
                            child->right->key,
                            (void *)root,
                            root->key
                        );
                        #endif
                        
                        _move_node(root, child -> right);
                        parent = root;

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: after move node into root %p ('%s') with new_left_branch at %p ('%s') \n",
                            (void *)root,
                            root->key,
                            (void *)new_left_branch,
                            new_left_branch ? new_left_branch->key : ""
                        );
                        #endif
                        
                    } else {
                        result = _destroy_node(child);

                        if (parent -> left == child) {
                            parent -> left = old_right_branch;
                        } else {
                            parent -> right = old_right_branch;
                        }

                        line_num = __LINE__ + 1;
                        rc = pthread_mutex_unlock(&(parent -> node_mutex));
                        if (rc)
                            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: released parent MUTEX at %p ('%s')\n",
                            (void *)&(parent -> node_mutex),
                            parent->key
                            );
                        #endif

                        parent = old_right_branch;

                    }

                    #ifdef VERBOSE
                    printf(
                        "utree_delete: new_left_branch=%p ('%s')\n",
                        (void *)new_left_branch,
                        new_left_branch ? new_left_branch->key : ""
                    );
                    #endif

                    for (tree_ptr_t left_branch = new_left_branch; left_branch != NULL; ) {
                        line_num = __LINE__ + 1;
                        rc = pthread_mutex_lock(&(left_branch -> node_mutex));
                        if (rc)
                            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: locked left_branch MUTEX at %p ('%s')\n",
                            (void *)&(left_branch -> node_mutex),
                            left_branch -> key
                            );
                        #endif

                        line_num = __LINE__ + 1;
                        rc = pthread_mutex_unlock(&(parent -> node_mutex));
                        if (rc)
                            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: released parent MUTEX at %p ('%s')\n",
                            (void *)&(parent -> node_mutex),
                            parent -> key
                            );
                        #endif

                        #ifdef VERBOSE
                        tree_ptr_t grandparent = parent;
                        #endif
                        
                        tree_ptr_t node = left_branch;
                        parent = left_branch;
                        left_branch = node -> left;

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: grandparent=%p ('%s'), parent=%p ('%s'), child=%p ('%s')\n",
                            (void *)grandparent,
                            grandparent ? grandparent->key : "",
                            (void *)parent,
                            parent ? parent->key : "",
                            (void *)left_branch,
                            left_branch ? left_branch->key : ""
                        );
                        #endif

                        if (!left_branch) break;

                        line_num = __LINE__ + 1;
                        rc = pthread_mutex_lock(&(left_branch -> node_mutex));
                        if (rc)
                            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                        #ifdef VERBOSE
                        printf(
                            "utree_delete: locked child MUTEX at %p ('%s')\n",
                            (void *)&(left_branch -> node_mutex),
                            left_branch ? left_branch->key : ""
                            );
                        #endif
                    }

                    #ifdef VERBOSE
                    printf(
                        "utree_delete: parent=%p ('%s'), old_left_branch=%p ('%s')\n",
                        (void *)parent,
                        parent ? parent->key : "",
                        (void *)old_left_branch,
                        old_left_branch ? old_left_branch->key : ""
                    );
                    #endif

                    parent -> left = old_left_branch;

                    line_num = __LINE__ + 1;
                    rc = pthread_mutex_unlock(&(old_left_branch -> node_mutex));
                    if (rc)
                        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

                    #ifdef VERBOSE
                    printf(
                        "utree_delete: released old_left_branch MUTEX at %p ('%s')\n",
                        (void *)&(old_left_branch -> node_mutex),
                        old_left_branch ? old_left_branch->key : ""
                        );
                    #endif
                }
            }

            line_num = __LINE__ + 1;
            rc = pthread_mutex_unlock(&(parent -> node_mutex));
            if (rc)
                error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

            #ifdef VERBOSE
            printf(
                "utree_delete: released PARENT MUTEX at %p\n",
                (void *)&(parent -> node_mutex)
                );
            printf(
                "utree_delete: '%s' deleted\n",
                key
            );
            #endif

            return result;
        }

        if (parent) {
            line_num = __LINE__ + 1;
            rc = pthread_mutex_unlock(&(parent -> node_mutex));
            if (rc) {
                error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
            }

            #ifdef VERBOSE
            printf(
                "utree_delete: released PARENT MUTEX at %p\n",
                (void *)&(parent -> node_mutex)
                );
            #endif
        }
        
        if (key_cmp < 0) {
            if (!(child -> left)) {
                result = 0;
                break;
            } else {
                tree_ptr_t node   = child;
                parent = node;
                child  = node -> left;
            }
        } else {
            if (!(child -> right)) {
                result = 0;
                break;
            } else {
                tree_ptr_t node   = child;
                parent = node;
                child  = node -> right;
            }
        }

        line_num = __LINE__ + 1;
        rc = pthread_mutex_lock(&(child -> node_mutex));
        if (rc)
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

        #ifdef VERBOSE
        printf("utree_delete: locked CHILD MUTEX at %p\n", (void *)&(child -> node_mutex));
        #endif

    }

    if (child) {
        line_num = __LINE__ + 1;
        rc = pthread_mutex_unlock(&(child -> node_mutex));
        if (rc) {
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);
        }

        #ifdef VERBOSE
        printf(
            "utree_delete: released CHILD MUTEX at %p\n",
            (void *)&(child -> node_mutex)
            );
        #endif
    }

    #ifdef VERBOSE
    printf(
        "utree_delete: '%s' deleted\n",
        key
    );
    #endif

    return result;
}

#pragma endregion

#pragma region utree_lookup

// -----------------------------------------------------------------------------
// Lookup a key within a tree and return the associated value if found
// -----------------------------------------------------------------------------

Boolean utree_lookup(tree_ptr_t root, char *key, void **value) {
    int key_cmp, rc, line_num;
    tree_ptr_t parent, child, node;
    Boolean result;

    if (root == NULL || key == NULL || strlen(key) == 0 || value == NULL) {
        if (value)
           *value = NULL;
        return FALSE;
    }

    for (parent = NULL, child = root; ;) {
        line_num = __LINE__ + 1;
        if (!child)
            error(
                EXIT_FAILURE,
                EINVAL,
                "utree_lookup: (line #%d) CHILD is NULL for PARENT at %p\n",
                line_num,
                (void *)parent
            );

        line_num = __LINE__ + 1;
        rc = pthread_mutex_lock(&(child -> node_mutex));
        if (rc)
            error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

        #ifdef VERBOSE
        printf(
            "utree_lookup: locked CHILD MUTEX at %p\n",
            (void *)&(child -> node_mutex)
            );
        #endif

        if (parent) {
            line_num = __LINE__ + 1;
            rc = pthread_mutex_unlock(&(parent -> node_mutex));
            if (rc)
                error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

            #ifdef VERBOSE
            printf(
                "utree_lookup: released PARENT MUTEX at %p\n",
                (void *)&(parent -> node_mutex)
                );
            #endif

        }

        if (!(child -> key)) {

            #ifdef VERBOSE
            printf(
                "utree_lookup: NULL key found for node at %p\n",
                (void *)child
                );
            #endif

            if (value)
                *value = NULL;
            result = FALSE;
            break;
        }

        key_cmp = strcmp(key, child -> key);

        #ifdef VERBOSE
        printf(
            "utree_lookup: key='%s', child -> key (%p) = '%s'\n",
            key,
            (void *)&(child -> key),
            child -> key
        );
        #endif
        
        if (!key_cmp) {
            if (child -> value) {
                int value_len = strlen(child -> value);
                line_num = __LINE__ + 1;
                void *found_value = malloc(value_len+1);
                if (!found_value)
                    error(
                        EXIT_FAILURE,
                        ENOMEM,
                        ERR_MSG_FMT,
                        __FILE__,
                        line_num
                    );

                strcpy((char*)found_value, (char *)child -> value);
                *value = found_value;
            } else {
                *value = NULL;
            }
            result = TRUE;
            break;
        }

        if (key_cmp < 0) {
            if (child -> left) {
                node   = child;
                parent = node;
                child  = node -> left;
            } else {
                *value = NULL;
                result = FALSE;
                break;
            }
        } else {
            if (child -> right) {
                node   = child;
                parent = node;
                child  = node -> right;
            } else {
                *value = NULL;
                result = FALSE;
                break;
            }
        }

        #ifdef VERBOSE
        printf(
            "utree_lookup: descended one level with PARENT at %p and CHILD at %p\n",
            (void *)parent,
            (void *)child
            );
        #endif

    }

    line_num = __LINE__ + 1;
    rc = pthread_mutex_unlock(&(child -> node_mutex));
    if (rc)
        error(EXIT_FAILURE, rc, ERR_MSG_FMT, __FILE__, line_num);

    #ifdef VERBOSE
    printf(
        "utree_lookup: released CHILD MUTEX at %p\n",
        (void *)&(child -> node_mutex)
        );
    #endif

    return result;
}

#pragma endregion
