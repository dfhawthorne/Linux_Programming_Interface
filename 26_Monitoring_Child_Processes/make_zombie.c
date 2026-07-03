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

#include <signal.h>
#include <libgen.h>             /* For basename() declaration */
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
    pid_t childPid;
    
    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    setbuf(stdout, NULL);       /* Disable buffering of stdout */

    if (verbose) fprintf(stderr, "Get parent pid before fork()\n");
    
    printf("Parent PID=%ld\n", (long) getpid());

    switch (childPid = fork()) {
    case -1:
        errExit("fork");

    case 0:     /* Child: immediately exits to become zombie */
        printf("Child (PID=%ld) exiting\n", (long) getpid());
        _exit(EXIT_SUCCESS);

    default:    /* Parent */
        sleep(3);               /* Give child a chance to start and exit */
        snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
        rc = system(cmd);            /* View zombie child */
        
        if (verbose) fprintf(stderr, "First system() call returned %d\n", rc);

        /* Now send the "sure kill" signal to the zombie */

        if (kill(childPid, SIGKILL) == -1)
            errMsg("kill");
        sleep(3);               /* Give child a chance to react to signal */
        printf("After sending SIGKILL to zombie (PID=%ld):\n", (long) childPid);
        rc = system(cmd);            /* View zombie child again */
        
        if (verbose) fprintf(stderr, "Second system() call returned %d\n", rc);

        exit(EXIT_SUCCESS);
    }
}
