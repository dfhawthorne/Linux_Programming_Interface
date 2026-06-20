/* -------------------------------------------------------------------------- *
 * Exercise 24-3:                                                             *
 *                                                                            *
 * Assuming that we can modify the program source code, how could we get a    *
 * core dump of a process at a given moment in time, while letting the        *
 * process continue execution?                                                *
 * -------------------------------------------------------------------------- */
 
#define _GNU_SOURCE
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// Global variables shared by processes

unsigned int verbose = 0;   // Level of verbosity of messages for tracing

int main(int argc, char *argv[]) {
    unsigned int linenum = 0;       // Used for error messages to pinpoint failing code
    
    // The command argument, -v, enables verbose tracing
    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    /* ---------------------------------------------------------------------- *
     * Fork process into parent and child                                     *
     * ---------------------------------------------------------------------- */

    if (verbose) fprintf(stderr, "Parent: Before vfork()\n");

    linenum = __LINE__ + 1;
    pid_t pid = vfork();    // Save PID as a process-local variable

    /* ---------------------------------------------------------------------- *
     * After VFORK                                                            *
     * ---------------------------------------------------------------------- */

    switch (pid) {
        case -1:    // VFORK failed
            error_at_line(
                EXIT_FAILURE, errno, __FILE__, linenum,
                "Call to vfork() failed for parent"
                );
            break;

        case 0:     // Child process
            if (verbose) fprintf(stderr, "Child: Get process ID\n");
            linenum = __LINE__ + 1;
            pid_t child_pid = getpid();
            if (child_pid == -1) {
                error_at_line(
                    EXIT_FAILURE, errno, __FILE__, linenum,
                    "Call to getpid() failed for child"
                    );
            }
            if (verbose) fprintf(stderr, "Child: Signal QUIT to get a core dump\n");
            linenum = __LINE__ + 1;
            if (kill(child_pid, SIGQUIT) == -1) {
                error_at_line(
                    EXIT_FAILURE, errno, __FILE__, linenum,
                    "Call to kill() failed for child"
                    );
            }
            if (verbose) fprintf(stderr, "Child: Exiting\n");
            _exit(EXIT_FAILURE);
            break;
        
        default:    // Parent process
            if (verbose) fprintf(stderr, "Parent: vfork() successful\n");
            break;
    }

    // -------------------------------------------------------------------------
    // Clean up is done by parent
    // -------------------------------------------------------------------------

    wait(NULL);
    
    exit(EXIT_SUCCESS);
}
