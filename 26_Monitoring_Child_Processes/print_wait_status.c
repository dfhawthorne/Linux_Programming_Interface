/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2026.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

/* print_wait_status.c

   Dissect and print the process termination status returned by wait()
   and related calls.
*/
#define _GNU_SOURCE     /* Get strsignal() declaration from <string.h> */
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "print_wait_status.h"  /* Declaration of printWaitStatus() */
#include "tlpi_hdr.h"

/* NOTE: The following function employs printf(), which is not
   async-signal-safe (see Section 21.1.2). As such, this function is
   also not async-signal-safe (i.e., beware of calling it from a
   SIGCHLD handler). */

void                    /* Examine a wait() status using the W* macros */
printWaitStatus(const char *msg, siginfo_t status)
{
    if (msg != NULL)
        printf("%s", msg);

    if (WIFEXITED(status.si_status)) {
        printf("child exited, status=%d\n", WEXITSTATUS(status.si_status));

    } else if (WIFSIGNALED(status.si_status)) {
        printf("child killed by signal %d (%s)",
                WTERMSIG(status.si_status), strsignal(WTERMSIG(status.si_status)));
#ifdef WCOREDUMP        /* Not in SUSv3, may be absent on some systems */
        if (WCOREDUMP(status.si_status))
            printf(" (core dumped)");
#endif
        printf("\n");

    } else if (WIFSTOPPED(status.si_status)) {
        printf("child stopped by signal %d (%s)\n",
                WSTOPSIG(status.si_status), strsignal(WSTOPSIG(status.si_status)));

#ifdef WIFCONTINUED     /* SUSv3 has this, but older Linux versions and
                           some other UNIX implementations don't */
    } else if (WIFCONTINUED(status.si_status)) {
        printf("child continued\n");
#endif

    } else {            /* Should never happen */
        printf("what happened to this child? (status=%x)\n",
                (unsigned int) status.si_status);
    }

    // Print out additional fields from siginfo_t

    printf("\nAdditional fields from siginfo_t\n");
    printf("\tSignal number=%d\n", status.si_signo);
    printf("\tSignal code=%d\n", status.si_code);
    printf("\tSending process ID=%ld\n", (long)status.si_pid);
    printf("\tReal user ID of sending process=%ld\n", (long)status.si_uid);
    printf("\tMemory location which caused fault=%16p\n", status.si_addr);
}
