// Auxiliary program for Exercise 27-2

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    switch (argc) {
        case 0:
            printf("Ex_2_child: No argv[0] passed\n");
            errno = EINVAL;
            return -1;
            break;

        case 1:
            printf("%s: No arguments passed\n", argv[0]);
            return 0;
            break;

        default:
            printf("%s: Arguments passed\n", argv[0]);
            for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
                printf("\t%d: \"%s\"\n", arg_idx, argv[arg_idx]);
            }
            return 0;
            break;
    }
    return 0;
}