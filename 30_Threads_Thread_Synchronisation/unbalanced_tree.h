#ifndef UNBALANCED_TREE
#define UNBALANCED_TREE

#include <pthread.h>

typedef enum {
    TRUE,
    FALSE
} Boolean;

typedef struct tree {
    char *key;
    void *value;
    struct tree *left, *right;
    pthread_mutex_t mutex;
} tree_t, *tree_ptr_t;

int utree_initialise(tree_ptr_t);
int utree_add(tree_ptr_t, char *, void *);
int utree_delete(tree_ptr_t, char *);
Boolean utree_lookup(tree_ptr_t, char *, void *);

#endif
