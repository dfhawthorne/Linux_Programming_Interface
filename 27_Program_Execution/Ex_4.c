// -----------------------------------------------------------------------------
// Exercise 27-4:
//
// What is the effect of the following code? In what circumstances might it be
// useful?
// -----------------------------------------------------------------------------

#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int verbose = 0;
    int line_num = 0;
    int status = 0;
    sigset_t user_signal_set;

    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    if (verbose) fprintf(stderr, "Parent blocks SIGUSR1\n");

    line_num = __LINE__ + 1;
    if (sigemptyset(&user_signal_set))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigemptyset() failed"
        );

    line_num = __LINE__ + 1;
    if (sigaddset(&user_signal_set, SIGUSR1))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigaddset() failed"
        );

    line_num = __LINE__ + 1;
    if (sigprocmask(SIG_BLOCK, &user_signal_set, NULL))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigprocmask() failed"
        );
    
    if (verbose) fprintf(stderr, "Parent spawns child\n");

    line_num = __LINE__ + 1;
    pid_t childPid = fork();
    if (childPid == -1)
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "fork() failed"
        );
    if (childPid == 0) {    // child
        if (verbose) fprintf(stderr, "Child spawns grandchild\n");
        line_num = __LINE__ + 1;
        switch (fork()) {
            case -1:
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "fork() failed"
                );
                break;
            
            case 0:         // grandchild
                if (verbose) fprintf(stderr, "Grandchild does real work\n");

                line_num = __LINE__ + 1;
                switch (sigwaitinfo(&user_signal_set, NULL)) {
                    case -1:
                        error_at_line(
                            EXIT_FAILURE,
                            errno,
                            __FILE__,
                            line_num,
                            "sigwaitinfo() failed"
                        );
                        break;
                    default:
                        if (verbose) fprintf(stderr, "Grandchild received SIGUSR1. Terminating...\n");
                        break;
                    }

                // do real work here
                exit(EXIT_SUCCESS);
                break;
            
            default:
                if (verbose) fprintf(stderr, "Child makes grandchild an orphan\n");
                exit(EXIT_SUCCESS);     // make grandchild an orphan
                break;
        }
    }

    // Parent falls through to here
    if (verbose) fprintf(stderr, "Parent continues\n");

    line_num = __LINE__ + 1;
    if (waitpid(childPid, &status, 0) == -1)
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "waitpid() failed"
        );

    // Parent carries on to do other things

    if (verbose) fprintf(stderr, "Parent checks status of family tree\n");

    line_num = __LINE__ + 1;
    int rc = system("ps -fC Ex_4");
    if (rc == -1)
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "system() failed"
        );

    return 0;
}