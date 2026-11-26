// -----------------------------------------------------------------------------
// Exercise 30-2:
//
// Implement a set of thread-safe functions that update and search an unbalanced
// binary tree.
// -----------------------------------------------------------------------------

// #define VERBOSE
// #define UNTHREADED

#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unbalanced_tree.h"
#include "Ex_2.h"

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

typedef struct search_results {
    int num_searched, num_found, num_matched;
} search_results_t, *search_results_p;

// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------

static tree_t root;
#include "Ex_2_test_data.c"
static int verbose = 0;

// -----------------------------------------------------------------------------
// Ancilliary functions for test harnass
// -----------------------------------------------------------------------------


static void print_dfs_node(const tree_t *node, size_t depth)
{
    if (!node)
        return;

    for (size_t i = 0; i < depth; ++i)
        fputs("    ", stdout);

    printf("%s\n", (node->key ? node->key : "(null)"));

    print_dfs_node(node->left,  depth + 1);
    print_dfs_node(node->right, depth + 1);
}

static char *lookup_dict(char *key) {
    if (!key) return NULL;
    for (int i; i < TOTAL_NUM_KEYS; i++) {
        if (!strcmp(data_dict.keys[i], key)) return data_dict.values[i];
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// Thread function to search a binary tree
// -----------------------------------------------------------------------------

static void *search_keys(void *arg) {
    search_args_t *arg_ptr = (search_args_t *) arg;
    int j;
    Boolean found;

    arg_ptr -> num_found = 0;

    if (verbose)
        printf("search_keys: thread #%d started: root=%p\n",
            arg_ptr -> thread_number,
            (void *)arg_ptr -> root_p
        );

    for (j = 0; j < arg_ptr -> num_keys; j++) {

        if (verbose)
            printf(
                "Thread #%d is searching for \"%s\"\n",
                arg_ptr -> thread_number,
                arg_ptr -> keys[j]
            );

        found = utree_lookup(
            arg_ptr -> root_p,
            (void *)arg_ptr -> keys[j],
            (void *)&(arg_ptr -> returned_values[j])
        );
        if (found) {
            arg_ptr -> num_found++;
            if (!strcmp(arg_ptr -> returned_values[j], arg_ptr -> expected_values[j]))
                arg_ptr -> num_matched++;
        }
        
        if (verbose)
            printf("search_keys: thread #%d key '%s' was %sfound\n",
                arg_ptr -> thread_number,
                arg_ptr -> keys[j],
                (found?"":"not ")
            );

    }

    if (verbose)
        printf("search_keys: thread #%d completed: root=%p, num_found=%d\n",
            arg_ptr -> thread_number,
            (void *)arg_ptr -> root_p,
            arg_ptr -> num_found
        );

    return NULL;
}

// -----------------------------------------------------------------------------
// Thread function to delete selected keys from a binary tree
// -----------------------------------------------------------------------------

static void *delete_keys(void *arg) {
    search_args_t *arg_ptr = (search_args_t *) arg;
    int j;

    arg_ptr -> num_found = 0;

    if (verbose)
        printf("delete_keys: thread #%d started: root=%p\n",
            arg_ptr -> thread_number,
            (void *)arg_ptr -> root_p
        );

    for (j = 0; j < arg_ptr -> num_keys; j++) {

        if (verbose)
            printf(
                "Thread #%d is deleting \"%s\"\n",
                arg_ptr -> thread_number,
                arg_ptr -> keys[j]
            );

        int rc = utree_delete(
            arg_ptr -> root_p,
            (void *)arg_ptr -> keys[j]
        );
        if (!rc) {
            arg_ptr -> num_found++;

            if (verbose)
                printf("delete_keys: thread #%d key '%s' was deleted\n",
                    arg_ptr -> thread_number,
                    arg_ptr -> keys[j]
                );
        } else {
            error(
                EXIT_FAILURE,
                rc,
                "utree_delete failed for key='%s'",
                arg_ptr -> keys[j]
            );
        }
    }

    if (verbose)
        printf("delete_keys: thread #%d completed: root=%p, num_found=%d\n",
            arg_ptr -> thread_number,
            (void *)arg_ptr -> root_p,
            arg_ptr -> num_found
        );

    return NULL;
}

// -----------------------------------------------------------------------------
// Driver to spawn threads to search a binary tree
// -----------------------------------------------------------------------------

search_results_t search_tree_3_thds(tree_ptr_t root_p, search_args_t *args) {
    search_results_t results = {
        .num_found = 0,
        .num_matched = 0,
        .num_searched = 0
    };
    pthread_t threads[NUM_THDS];
    int thd_idx = 0;
    int line_num = 0;

    for (thd_idx = 0; thd_idx < NUM_THDS; thd_idx++) {
        int rc;

        args[thd_idx].root_p = root_p; 

        memset(args[thd_idx].returned_values, 0, NUM_KEYS * sizeof(char *));
        line_num = __LINE__ + 1;
        rc = pthread_create(
            &threads[thd_idx], 
            NULL, 
            search_keys, 
            &args[thd_idx]
        );
        if (rc) {
            for (int j = 0; j < thd_idx; j++) {
                pthread_join(
                    threads[j],
                    NULL
                );
            }
            error_at_line(
                EXIT_FAILURE,
                rc,
                __FILE__,
                line_num,
                "Unable to create thread #%d",
                thd_idx
            );
        } 
        else {
            if (verbose)
                printf(
                    "search_tree_3_thds: thread #%d created successfully\n",
                    thd_idx
                );
        }
    }

    for (thd_idx = 0; thd_idx < NUM_THDS; thd_idx++) {
        int rc;

        line_num = __LINE__ + 1;
        rc = pthread_join(
            threads[thd_idx],
            NULL
        );
        if (rc)
            error_at_line(
                EXIT_FAILURE,
                rc,
                __FILE__,
                line_num,
                "Unable to join thread #%d",
                thd_idx
            );
        results.num_found += args[thd_idx].num_found;
        results.num_matched += args[thd_idx].num_matched;
        results.num_searched += NUM_KEYS;
        for (int idx = 0; idx < NUM_KEYS; idx++) {
            if (args[thd_idx].returned_values[idx]) {
                char *dict_value = lookup_dict(args[thd_idx].keys[idx]);
                if (dict_value) {
                    if (!strcmp(args[thd_idx].returned_values[idx],dict_value))
                        results.num_matched++;
                }
                free(args[thd_idx].returned_values[idx]);
            }
        }
    }
    
    return results;
}

// -----------------------------------------------------------------------------
// Driver to spawn threads to delete selected keys from a binary tree
// -----------------------------------------------------------------------------

search_results_t delete_tree_3_thds(tree_ptr_t root_p) {
    search_results_t results = {
        .num_found = 0,
        .num_matched = 0,
        .num_searched = 0
    };
    pthread_t threads[NUM_THDS];
    int thd_idx = 0;
    int line_num = 0;

    search_args_t args[NUM_THDS] = {
        { 
            .thread_number = 0, 
            .root_p = root_p, 
            .num_keys = NUM_KEYS,
            .keys = { "Apple", "Banana", "Cat" },
            .num_found = 0
        },
        { 
            .thread_number = 1, 
            .root_p = root_p, 
            .num_keys = NUM_KEYS,
            .keys = { "Diamond", "Echo", "Fox" },
            .num_found = 0
        },
        { 
            .thread_number = 2, 
            .root_p = root_p, 
            .num_keys = NUM_KEYS,
            .keys = { "Golf", "Banana", "Fox" },
            .num_found = 0
        }
    };

    for (thd_idx = 0; thd_idx < NUM_THDS; thd_idx++) {
        int rc;

        memset(args[thd_idx].returned_values, 0, NUM_KEYS * sizeof(char *));
        line_num = __LINE__ + 1;
        rc = pthread_create(
            &threads[thd_idx], 
            NULL, 
            delete_keys, 
            &args[thd_idx]
        );
        if (rc) {
            for (int j = 0; j < thd_idx; j++) {
                pthread_join(
                    threads[j],
                    NULL
                );
            }
            error_at_line(
                EXIT_FAILURE,
                rc,
                __FILE__,
                line_num,
                "Unable to create thread #%d",
                thd_idx
            );
        } 
        else {
            if (verbose)
                printf(
                    "delete_tree_3_thds: thread #%d created successfully\n",
                    thd_idx
                );
        }
    }

    for (thd_idx = 0; thd_idx < NUM_THDS; thd_idx++) {
        int rc;

        line_num = __LINE__ + 1;
        rc = pthread_join(
            threads[thd_idx],
            NULL
        );
        if (rc)
            error_at_line(
                EXIT_FAILURE,
                rc,
                __FILE__,
                line_num,
                "Unable to join thread #%d",
                thd_idx
            );
        results.num_found += args[thd_idx].num_found;
        results.num_searched += NUM_KEYS;
    }
    
    return results;
}

// -----------------------------------------------------------------------------
// Driver to search a binary tree in unthreaded mode.
//   Used to test functionality without introducing problems with threading.
// -----------------------------------------------------------------------------

search_results_t search_tree(tree_ptr_t root_p, int num_keys, int key_order[]) {
    search_results_t results = {
        .num_found = 0,
        .num_matched = 0,
        .num_searched = 0
    };
    char *found_value;

    for (int i = 0; i < num_keys; i++) {
        results.num_searched++;
        int idx = key_order[i];
        if (verbose)
            printf(
                "search_tree: #%d key[%d]='%s'\n",
                i,
                idx,
                data_dict.keys[idx]
            );
        Boolean found = utree_lookup(root_p, data_dict.keys[idx], (void *)&found_value);
        if (found) {
            results.num_found++;
            if (strcmp(found_value, data_dict.values[idx]))
                printf(
                    "utree_lookup for key='%s' returned '%s' instead of '%s'\n",
                    data_dict.keys[idx],
                    found_value,
                    data_dict.values[idx]
                );
            else
                results.num_matched++;
        } else 
            printf(
                "utree_lookup did not find key='%s'\n",
                data_dict.keys[idx]
            );
    }

    return results;
}

// -----------------------------------------------------------------------------
// Driver to delete selected keys from a binary tree in unthreaded mode.
//   Used to test functionality without introducing problems with threading.
// -----------------------------------------------------------------------------

search_results_t delete_tree(tree_ptr_t root_p, int num_keys, int key_order[]) {
    search_results_t results = {
        .num_found = 0,
        .num_matched = 0,
        .num_searched = 0
    };

    if (verbose) {
        printf(
            "delete_tree: root_p=%p, num_keys=%d\n",
            (void *)root_p,
            num_keys
        );
        print_dfs_node(root_p, 0);
    }

    for (int i = 0; i < num_keys; i++) {
        results.num_searched++;
        int idx = key_order[i];
        if (verbose)
            printf(
                "delete_tree: #%d key[%d]='%s'\n",
                i,
                idx,
                data_dict.keys[idx]
            );
        int rc = utree_delete(root_p, data_dict.keys[idx]);
        if (rc)
            error(
                EXIT_FAILURE,
                rc,
                "utree_delete failed for key='%s' and value='%s'",
                data_dict.keys[idx],
                data_dict.values[idx]
            );
        results.num_matched++;
        results.num_found++;
        if (verbose)
            print_dfs_node(root_p, 0);
    }

    return results;
}

// -----------------------------------------------------------------------------
// Test Case #1: Argument Validation
// -----------------------------------------------------------------------------

void test_case_1(void) {
    int rc = 0;
    const char *key = "Zebra", *value = "Animal";
    tree_ptr_t root_p = &root;

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
// Test Case #2: Searching Empty Trees
// -----------------------------------------------------------------------------

void test_case_2(tree_ptr_t root_p) {
    search_results_t results = search_tree_3_thds(root_p, known_args_3_thds);
    if (results.num_found) {
        fprintf(
            stderr,
            "Tree search found %d results. Expected 0.\n",
            results.num_found
        );
        exit(EXIT_FAILURE);
    }
}

// -----------------------------------------------------------------------------
// Test Case #3: Deleting Empty Trees
// -----------------------------------------------------------------------------

void test_case_3(tree_ptr_t root_p) {
    #ifdef UNTHREADED
    search_results_t results = delete_tree(root_p, NUM_TREE_NODES, bal_key_order);
    if (results.num_found != NUM_TREE_NODES) {
        fprintf(
            stderr,
            "Tree search found %d results. Expected %d.\n",
            results.num_found,
            TOTAL_NUM_KEYS
        );
        exit(EXIT_FAILURE);
    }
    #else
    search_results_t results = delete_tree_3_thds(root_p);
    if (results.num_found != TOTAL_NUM_KEYS) {
        fprintf(
            stderr,
            "Tree search found %d results. Expected %d.\n",
            results.num_found,
            TOTAL_NUM_KEYS
        );
        exit(EXIT_FAILURE);
    }
    #endif
}

// -----------------------------------------------------------------------------
// Test Cases #4-6: Manipulating a balanced, left-skewed, and right-skewed tree
// Sub-cases:
// ?-1: Build ?? Tree
// ?-2: Search tree
// ?-3: Search for non-existent key
// ?-4: Insert duplicate key
// ?-5: Delete then insert an existing key
// ?-6: Delete all keys
// -----------------------------------------------------------------------------

void build_tree(tree_ptr_t root_p, int num_keys, int key_order[], int test_case_no, char *tree_type_str) {
    printf("Test case #%d-1: (Build %s Tree) started.\n", test_case_no, tree_type_str);
    for (int i = 0; i < num_keys; i++) {
        int idx = key_order[i];
        int rc = utree_add(root_p, data_dict.keys[idx], data_dict.values[idx]);
        if (rc) {
            error(
                EXIT_FAILURE,
                rc,
                "utree_add failed for key='%s' and value='%s'",
                data_dict.keys[idx],
                data_dict.values[idx]
            );
        }
    }
    printf("Test case #%d-1: (Build %s Tree) completed successfully.\n", test_case_no, tree_type_str);
}

void test_case_for_tree(tree_ptr_t root_p, int test_case_no) {
    search_results_t results;
    printf("Test case #%d-2: (Search tree) started.\n", test_case_no);

    #ifdef UNTHREADED
    results = search_tree(root_p, NUM_TREE_NODES, bal_key_order);
    if (results.num_matched != NUM_TREE_NODES) {
        fprintf(
            stderr,
            "Tree search matched %d results. Expected %d.\n",
            results.num_matched,
            TOTAL_NUM_KEYS
        );
        exit(EXIT_FAILURE);
    }
    #else
    results = search_tree_3_thds(root_p, known_args_3_thds);
    if (results.num_matched != TOTAL_NUM_KEYS) {
        fprintf(
            stderr,
            "Tree search matched %d results. Expected %d.\n",
            results.num_matched,
            TOTAL_NUM_KEYS
        );
        exit(EXIT_FAILURE);
    }
    #endif

    printf("Test case #%d-2: (Search tree) completed successfully.\n", test_case_no);
    printf("Test case #%d-3: (Search for non-existent key) started.\n", test_case_no);

    results = search_tree_3_thds(root_p, unknown_args_3_thds);
    if (results.num_matched != 0) {
        fprintf(
            stderr,
            "Tree search matched %d results. Expected 0.\n",
            results.num_matched
        );
        exit(EXIT_FAILURE);
    }

    printf("Test case #%d-3: (Search for non-existent key) completed successfully.\n", test_case_no);
    printf("Test case #%d-4: (Insert duplicate key) started.\n", test_case_no);
    printf("Test case #%d-4: (Insert duplicate key) completed successfully.\n", test_case_no);
    printf("Test case #%d-5: (Delete then insert an existing key) started.\n", test_case_no);
    printf("Test case #%d-5: (Delete then insert an existing key) completed successfully.\n", test_case_no);
    printf("Test case #%d-6: (Delete all keys) started.\n", test_case_no);

    #ifdef UNTHREADED
    results = delete_tree(root_p, NUM_TREE_NODES, bal_key_order);
    if (results.num_found != NUM_TREE_NODES) {
        fprintf(
            stderr,
            "Tree deletion found %d results. Expected %d.\n",
            results.num_found,
            TOTAL_NUM_KEYS
        );
        exit(EXIT_FAILURE);
    }
    #else
    results = delete_tree_3_thds(root_p);
    if (results.num_found != TOTAL_NUM_KEYS) {
        fprintf(
            stderr,
            "Tree deletion found %d results. Expected %d.\n",
            results.num_found,
            TOTAL_NUM_KEYS
        );
        exit(EXIT_FAILURE);
    }
    #endif

    printf("Test case #%d-6: (Delete all keys) completed successfully.\n", test_case_no);
}

// -----------------------------------------------------------------------------
// Main program
// -----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int line_num = 0;
    int rc = 0;

    #ifdef VERBOSE
    if (setvbuf(stdout, NULL, _IONBF, 0))
        return EXIT_FAILURE;
    #endif

    if (argc > 1 && !strcmp(argv[1],"-v")) verbose = 1;

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
    
    printf("Test Case #2 (Searching Empty Trees) started.\n");
    test_case_2(&root);
    printf("Test Case #2 (Searching Empty Trees) completed successfully.\n");
    
    printf("Test Case #3 (Deleting Empty Trees) started.\n");
    test_case_3(&root);
    printf("Test Case #3 (Deleting Empty Trees) completed successfully.\n");
    
    printf("Test Case #4 (Manipulating Balanced Tree) started.\n");
    build_tree(&root, NUM_TREE_NODES, bal_key_order, 4, "balanced");
    test_case_for_tree(&root, 4);
    printf("Test Case #4 (Manipulating Balanced Tree) completed successfully.\n");
    
    exit(EXIT_SUCCESS);
}