// -----------------------------------------------------------------------------
// Exercise 30-2:
//
// Implement a set of thread-safe functions that update and search an unbalanced
// binary tree.
// -----------------------------------------------------------------------------

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

#include "unbalanced_tree.h"

static tree_t root;

// -----------------------------------------------------------------------------
// Ancilliary functions for test harnass
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Case #1: Argument Validation
// -----------------------------------------------------------------------------

void test_case_1(void) {
    int rc = 0;
    const char *key = "Zebra", *value = "Animal";
    tree_t *root_p = &root;

    rc = utree_initialise(NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_initialise(NULL)");
    rc = utree_add(NULL, NULL, NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(NULL, NULL, NULL)");
    rc = utree_add(NULL, (void *)key, NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(NULL, key, NULL)");
    rc = utree_add(NULL, (void *)"", NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(NULL, """", NULL)");
    rc = utree_add(NULL, NULL, (void *)value);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(NULL, NULL, value)");
    rc = utree_add(root_p, NULL, NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(root_p, NULL, NULL)");
    rc = utree_add(root_p, (void *)key, NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(root_p, key, NULL)");
    rc = utree_add(root_p, (void *)"", NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(root_p, """", NULL)");
    rc = utree_add(root_p, NULL, (void *)value);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_add(root_p, NULL, value)");
    rc = utree_delete(NULL, NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_delete(NULL, NULL)");
    rc = utree_delete(root_p, NULL);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_delete(root_p, NULL)");
    rc = utree_delete(NULL, (void *)key);
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_delete(NULL, key)");
    rc = utree_delete(NULL, (void *)"");
    if (rc != EINVAL)
        error(EXIT_FAILURE, rc, "Unexpected RC from utree_delete(NULL, """")");
}

// -----------------------------------------------------------------------------
// Main program
// -----------------------------------------------------------------------------

int main() {
    int line_num = 0;
    int rc = 0;

    line_num = __LINE__ + 1;
    rc = utree_initialise(&root);
    if (rc)
        error_at_line(
            EXIT_FAILURE,
            rc,
            __FILE__,
            line_num,
            "utree_initialise() failed"
        );
    
    printf("Test Case #1 (Argument Validation) started.\n");
    test_case_1();
    printf("Test Case #1 (Argument Validation) completed successfully.\n");
    
    exit(EXIT_SUCCESS);
}