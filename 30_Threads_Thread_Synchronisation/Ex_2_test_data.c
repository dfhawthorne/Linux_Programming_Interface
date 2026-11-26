// ----------------------------------------------------------------------------
// Test data for Ex 30-2
// ----------------------------------------------------------------------------

#include <stdlib.h>
#include "Ex_2.h"

// ----------------------------------------------------------------------------
// A data dictionary of key-value pairs that to be used as vertices
// ----------------------------------------------------------------------------

static data_dict_t data_dict = {
    .keys   = {"Apple", "Banana", "Cat", "Diamond", "Echo", "Fox", "Golf"},
    .values = {"Fruit", "Berry", "Animal", "Mineral", "Sound", "Animal", "Game"}
};

// ----------------------------------------------------------------------------
// Argument arrays for 3 threads
// ----------------------------------------------------------------------------

search_args_t known_args_3_thds[NUM_THDS] = {
    { 
        .thread_number = 0, 
        .root_p = NULL, 
        .num_keys = NUM_KEYS,
        .keys = { "Apple", "Banana", "Cat" },
        .expected_values = {data_dict.values[0], data_dict.values[1], data_dict.values[2]},
        .num_matched = 0,
        .num_found = 0
    },
    { 
        .thread_number = 1, 
        .root_p = NULL, 
        .num_keys = NUM_KEYS,
        .keys = { "Diamond", "Echo", "Fox" },
        .expected_values = {data_dict.values[3], data_dict.values[4], data_dict.values[5]},
        .num_matched = 0,
        .num_found = 0
    },
    { 
        .thread_number = 2, 
        .root_p = NULL, 
        .num_keys = NUM_KEYS,
        .keys = { "Golf", "Banana", "Fox" },
        .expected_values = {data_dict.values[6], data_dict.values[1], data_dict.values[5]},
        .num_matched = 0,
        .num_found = 0
    }
};

search_args_t unknown_args_3_thds[NUM_THDS] = {
    { 
        .thread_number = 0, 
        .root_p = NULL, 
        .num_keys = NUM_KEYS,
        .keys = { "India", "Juliet", "Kilo" },
        .expected_values = {NULL, NULL, NULL},
        .num_matched = 0,
        .num_found = 0
    },
    { 
        .thread_number = 1, 
        .root_p = NULL, 
        .num_keys = NUM_KEYS,
        .keys = { "Lima", "Mike", "November" },
        .expected_values = {NULL, NULL, NULL},
        .num_matched = 0,
        .num_found = 0
    },
    { 
        .thread_number = 2, 
        .root_p = NULL, 
        .num_keys = NUM_KEYS,
        .keys = { "Oscar", "Papa", "Romeo" },
        .expected_values = {NULL, NULL, NULL},
        .num_matched = 0,
        .num_found = 0
    }
};

// ----------------------------------------------------------------------------
// Trees expressed as a sequence in which the vertices are inserted
// ----------------------------------------------------------------------------

static int bal_key_order[NUM_TREE_NODES] = {3, 1, 0, 2, 5, 4, 6};
