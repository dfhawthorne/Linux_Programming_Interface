// ----------------------------------------------------------------------------
// Exercise 26-4:
// Listing 26-4 (make_zombie.c) uses a call to sleep() to allow the child
// process a chance to execute and terminate before the parent executes
// system(). This approach produces a theoretical race condition. Modify the
// program to eliminate the race condition by using signals to synchronize the
// parent and child.
// ----------------------------------------------------------------------------


/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2026.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* make_zombie.c

   Demonstrate how a child process becomes a zombie in the interval between
   the time it exits, and the time its parent performs a wait (or exits, at
   which time it is adopted by init(8), which does a wait, thus releasing
   the zombie).
*/

#define _GNU_SOURCE

#include <error.h>
#include <errno.h>
#include <libgen.h>             /* For basename() declaration */
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "tlpi_hdr.h"

#define CMD_SIZE 200

int
main(int argc, char *argv[])
{
    int rc = 0;
    int verbose = 0;
    char cmd[CMD_SIZE];
    pid_t childPid = 0;
    pid_t parent_pid = 0;
    int line_num = 0;
    int recv_sig = 0;
    sigset_t sig_set;
    siginfo_t sig_info;
    
    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    setbuf(stdout, NULL);       /* Disable buffering of stdout */

    if (verbose) fprintf(stderr, "Get parent pid before fork()\n");
    
    line_num = __LINE__ + 1;
    parent_pid = getpid();
    if (parent_pid == -1)
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "getpid() failed"
        );
    printf("Parent PID=%ld\n", (long) parent_pid);

    if (verbose) fprintf(stderr, "Create empty signal set\n");
    line_num = __LINE__ + 1;
    if (sigemptyset(&sig_set))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigemptyset() failed"
            );

    if (verbose) fprintf(stderr, "Add SIGUSR1 to signal set\n");
    line_num = __LINE__ + 1;
    if (sigaddset(&sig_set, SIGUSR1))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigaddset() failed"
            );

    if (verbose) fprintf(stderr, "Block SIGUSR1\n");
    line_num = __LINE__ + 1;
    if (sigprocmask(SIG_BLOCK, &sig_set, NULL))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigprocmask() failed"
            );

    switch (childPid = fork()) {
    case -1:
        errExit("fork");

    case 0:     /* Child: immediately exits to become zombie */
        if (verbose) fprintf(stderr, "Child started\n");

        if (verbose) fprintf(stderr, "Child sends SIGUSR1 to parent\n");
        line_num = __LINE__ + 1;
        if (kill(parent_pid, SIGUSR1) == -1)
            error_at_line(
                EXIT_FAILURE,
                errno,
                __FILE__,
                line_num,
                "kill() failed"
                );

        printf("Child (PID=%ld) exiting\n", (long) getpid());
        _exit(EXIT_SUCCESS);

    default:    /* Parent */
        if (verbose) fprintf(stderr, "Parent continues\nChild PID=%ld\n", (long)childPid);

        line_num = __LINE__ + 1;
        switch (recv_sig = sigwaitinfo(&sig_set, &sig_info)) {
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
                if (verbose)
                    fprintf(
                        stderr,
                        "Received signal %d from process with PID=%ld\n",
                        sig_info.si_signo,
                        (long)sig_info.si_pid
                        );
                break;
            }

        if (verbose) fprintf(stderr, "Parent checks status of processes\n");
        snprintf(cmd, CMD_SIZE, "ps -fC %s", basename(argv[0]));
        if (verbose) fprintf(stderr, "Command to execute is '%s'\n", cmd);
        rc = system(cmd);            /* View zombie child */
        
        if (verbose) fprintf(stderr, "First system() call returned %d\n", rc);

        /* Now send the "sure kill" signal to the zombie */

        if (kill(childPid, SIGKILL) == -1)
            errMsg("kill");
        printf("After sending SIGKILL to zombie (PID=%ld):\n", (long) childPid);

        sleep(5);                    // allow child time to clean up
        rc = system(cmd);            /* View zombie child again */
        
        if (verbose) fprintf(stderr, "Second system() call returned %d\n", rc);

        exit(EXIT_SUCCESS);
    }
}
