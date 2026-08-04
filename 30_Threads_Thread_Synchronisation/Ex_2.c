// -----------------------------------------------------------------------------
// Exercise 30-2:
//
// -----------------------------------------------------------------------------

#include <error.h>
#include <stdlib.h>

#include "unbalanced_tree.h"

static tree_t root;

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
    
    exit(EXIT_SUCCESS);
}