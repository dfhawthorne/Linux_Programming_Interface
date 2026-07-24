// -----------------------------------------------------------------------------
// Exercise 27-3:
//
// -----------------------------------------------------------------------------

#define _GNU_SOURCE
#define MAX_INT_STR_SZ 10

#include <errno.h>
#include <error.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int verbose = 0;

int main(int argc, char *argv[]) {
    int line_num = 0;

    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    line_num = __LINE__ + 1;
    pid_t child_pid = fork();
    switch (child_pid) {
        case -1:
            error_at_line(
                EXIT_FAILURE,
                errno,
                __FILE__,
                line_num,
                "fork() failed"
            );
            break;
        case 0:     // child process
            if (verbose) fprintf(stderr, "Child started\n");
            line_num = __LINE__ + 1;
            int rc = execlp("./Ex_3.sh", "./Ex_3.sh", (char *)NULL);
            if (rc == -1) {
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "my_execlp() failed"
                );
                _exit(EXIT_FAILURE);
            }
            break;
        default:    // parent process
            printf("Parent spawned PID=%ld\n", (long)child_pid);
            wait(NULL);
            break;
    }
    return 0;
}
