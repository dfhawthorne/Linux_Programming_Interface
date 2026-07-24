// Auxiliary program for Exercise 27-2

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int line_num = 0;
    char *user_name = NULL;

    switch (argc) {
        case 0:
            printf("Ex_2_child: No argv[0] passed\n");
            errno = EINVAL;
            return -1;
            break;

        case 1:
            printf("%s: No arguments passed\n", argv[0]);
            break;

        default:
            printf("%s: Arguments passed\n", argv[0]);
            for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
                printf("\t%d: \"%s\"\n", arg_idx, argv[arg_idx]);
            }
            break;
    }

    line_num = __LINE__ + 1;
    if ((user_name = getenv("USER")) == NULL) {
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "getenv() failed"
        );
    }
    printf("Environment variable (\"USER\") has value (\"%s\")\n", user_name);

    return 0;
}