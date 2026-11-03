// -----------------------------------------------------------------------------
// Exercise 30-2:
//
// Implement a set of thread-safe functions that update and search an unbalanced
// binary tree.
// -----------------------------------------------------------------------------

// #define VERBOSE

#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unbalanced_tree.h"
#define MAX_KEY_LEN 16
#define MAX_VALUE_LEN 32
#define MAX_NUM_KEYS 3
#define TOTAL_NUM_KEYS 9
#define NUM_THDS 3
#define NUM_KEYS 3

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

typedef struct search_args {
    int thread_number;
    tree_t *root_p;
    char keys[MAX_NUM_KEYS][MAX_KEY_LEN+1];
    char *values[MAX_NUM_KEYS];
    int num_keys, num_found;
} search_args_t;

typedef struct data_dict {
    char keys[TOTAL_NUM_KEYS][MAX_KEY_LEN+1];
    char values[TOTAL_NUM_KEYS][MAX_VALUE_LEN+1];
} data_dict_t;

typedef struct search_results {
    int num_searched, num_found, num_matched;
} search_results_t, *search_results_p;

// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------

static tree_t root;
static data_dict_t data_dict = {
    .keys   = {"Apple", "Banana", "Cat", "Diamond", "Echo", "Fox", "Golf"},
    .values = {"Fruit", "Berry", "Animal", "Mineral", "Sound", "Animal", "Game"}
};

// -----------------------------------------------------------------------------
// Ancilliary functions for test harnass
// -----------------------------------------------------------------------------

static char *lookup_dict(char *key) {
    if (key == NULL) return NULL;
    for (int i; i < TOTAL_NUM_KEYS; i++) {
        int cmp = strcmp(data_dict.keys[i], key);
        if (cmp == 0) return data_dict.values[i];
    }
    return NULL;
}

static void *search_keys(void *arg) {
    search_args_t *arg_ptr = (search_args_t *) arg;
    int j;
    Boolean found;

    arg_ptr -> num_found = 0;

    #ifdef VERBOSE
    printf("search_keys: thread #%d started:\n\troot_ptr=%p\n",
        arg_ptr -> thread_number,
        (void *)arg_ptr -> root_p);
    #endif

    for (j = 0; j < arg_ptr -> num_keys; j++) {

        #ifdef VERBOSE 
        printf(
            "Thread #%d is searching for \"%s\"\n",
            arg_ptr -> thread_number,
            arg_ptr -> keys[j]
        );
        #endif

        found = utree_lookup(
            arg_ptr -> root_p,
            (void *)arg_ptr -> keys[j],
            (void *)&(arg_ptr -> values[j])
        );
        if (found) {
            arg_ptr -> num_found++;

            #ifdef VERBOSE
            printf("search_keys: thread #%d key '%s' was %sfound\n",
                arg_ptr -> thread_number,
                arg_ptr -> keys[j],
                (found?"":"not ")
            );
            #endif
        }
    }

    return NULL;
}

static void *delete_keys(void *arg) {
    search_args_t *arg_ptr = (search_args_t *) arg;
    int j;
    Boolean found;

    arg_ptr -> num_found = 0;

    #ifdef VERBOSE
    printf("delete_keys: thread #%d started:\n\troot_ptr=%p\n",
        arg_ptr -> thread_number,
        (void *)arg_ptr -> root_p);
    #endif

    for (j = 0; j < arg_ptr -> num_keys; j++) {

        #ifdef VERBOSE 
        printf(
            "Thread #%d is searching for \"%s\"\n",
            arg_ptr -> thread_number,
            arg_ptr -> keys[j]
        );
        #endif

        found = utree_delete(
            arg_ptr -> root_p,
            (void *)arg_ptr -> keys[j]
        );
        if (found) {
            arg_ptr -> num_found++;

            #ifdef VERBOSE
            printf("delete_keys: thread #%d key '%s' was %sfound\n",
                arg_ptr -> thread_number,
                arg_ptr -> keys[j],
                (found?"":"not ")
            );
            #endif
        }
    }

    return NULL;
}

search_results_t search_tree_3_thds(tree_t *root_p) {
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

        memset(args[thd_idx].values, 0, NUM_KEYS * sizeof(char *));
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
        #ifdef VERBOSE
        else {
            printf("search_tree_3_thds: thread #%d created successfully\n", thd_idx);
        }
        #endif
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
        for (int idx = 0; idx < NUM_KEYS; idx++) {
            if (args[thd_idx].values[idx] != NULL) {
                char *dict_value = lookup_dict(args[thd_idx].keys[idx]);
                if (dict_value != NULL) {
                    int cmp = strcmp(args[thd_idx].values[idx],dict_value);
                    if (cmp == 0) results.num_matched++;
                }
                free(args[thd_idx].values[idx]);
            }
        }
    }
    
    return results;
}

search_results_t delete_tree_3_thds(tree_t *root_p) {
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

        memset(args[thd_idx].values, 0, NUM_KEYS * sizeof(char *));
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
        #ifdef VERBOSE
        else {
            printf("search_tree_3_thds: thread #%d created successfully\n", thd_idx);
        }
        #endif
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
// Test Case #2: Searching Empty Trees
// -----------------------------------------------------------------------------

void test_case_2(tree_t *root_p) {
    search_results_t results = search_tree_3_thds(root_p);
    if (results.num_found != 0) {
        fprintf(stderr, "Tree search found %d results. Expected 0.\n", results.num_found);
        exit(EXIT_FAILURE);
    }
}

// -----------------------------------------------------------------------------
// Test Case #3: Deleting Empty Trees
// -----------------------------------------------------------------------------

void test_case_3(tree_t *root_p) {
    search_results_t results = delete_tree_3_thds(root_p);
    if (results.num_found != 9) {
        fprintf(stderr, "Tree search found %d results. Expected 9.\n", results.num_found);
        exit(EXIT_FAILURE);
    }
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
    
    printf("Test Case #2 (Searching Empty Trees) started.\n");
    test_case_2(&root);
    printf("Test Case #2 (Searching Empty Trees) completed successfully.\n");
    
    printf("Test Case #3 (Deleting Empty Trees) started.\n");
    test_case_3(&root);
    printf("Test Case #3 (Deleting Empty Trees) completed successfully.\n");
    
    exit(EXIT_SUCCESS);
}