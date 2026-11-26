// ----------------------------------------------------------------------------
// Constants and types for Ex 30-2
// ----------------------------------------------------------------------------

#ifndef EX302CONST
    #define EX302CONST
    #define MAX_KEY_LEN 16
    #define MAX_VALUE_LEN 32
    #define MAX_NUM_KEYS 3
    #define TOTAL_NUM_KEYS 9
    #define NUM_THDS 3
    #define NUM_KEYS 3
    #define NUM_TREE_NODES 7

    typedef struct search_args {
        int thread_number;
        tree_ptr_t root_p;
        char keys[MAX_NUM_KEYS][MAX_KEY_LEN+1];
        char *returned_values[MAX_NUM_KEYS];
        char *expected_values[MAX_NUM_KEYS];
        int num_keys, num_found, num_matched;
    } search_args_t;

    typedef struct data_dict {
        char keys[TOTAL_NUM_KEYS][MAX_KEY_LEN+1];
        char values[TOTAL_NUM_KEYS][MAX_VALUE_LEN+1];
    } data_dict_t;

#endif
