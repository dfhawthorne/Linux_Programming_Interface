// -----------------------------------------------------------------------------
// Implements unbalanced tree for Exercise 30-2
// -----------------------------------------------------------------------------

#include <errno.h>
#include "unbalanced_tree.h"

int utree_initialise(tree_ptr_t root) {
    (void)root;
    return ENOSYS;
}

int utree_add(tree_ptr_t root, char *key, void *value) {
    (void)root;
    (void)key;
    (void)value;
    return ENOSYS;
}

int utree_delete(tree_ptr_t root, char *key) {
    (void)root;
    (void)key;
    return ENOSYS;
}

Boolean utree_lookup(tree_ptr_t root, char *key, void *value) {
    (void)root;
    (void)key;
    (void)value;
    value = NULL;
    return FALSE;
}
