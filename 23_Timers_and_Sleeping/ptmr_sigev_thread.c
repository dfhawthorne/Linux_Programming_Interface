/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2024.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/*************************************************************************
 * Ex 23-4: Modify ptmr_sigev_thread.c to use sigwaitinfo() instead of a *
 *          signal handler.                                              *
 *************************************************************************/

/* ptmr_sigev_thread.c

   This program demonstrates the use of threads as the notification mechanism
   for expirations of a POSIX timer. Each of the program's command-line
   arguments specifies the initial value and interval for a POSIX timer. The
   format of these arguments is defined by the function itimerspecFromStr().

   The program creates and arms one timer for each command-line argument.
   The timer notification method is specified as SIGEV_THREAD, causing the
   timer notifications to be delivered via a thread that invokes threadFunc()
   as its start function. The threadFunc() function displays information
   about the timer expiration, increments a global counter of timer expirations,
   and signals a condition variable to indicate that the counter has changed.
   In the main thread, a loop waits on the condition variable, and each time
   the condition variable is signaled, the main thread prints the value of the
   global variable that counts timer expirations.

   Kernel support for Linux timers is provided since Linux 2.6. On older
   systems, an incomplete user-space implementation of POSIX timers
   was provided in glibc.
*/

#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <error.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "curr_time.h"              /* Declares currTime() */
#include "tlpi_hdr.h"
#include "itimerspec_from_str.h"    /* Declares itimerspecFromStr() */

int
main(int argc, char *argv[])
{
    struct sigevent sev;
    struct itimerspec ts;
    timer_t *tidlist;
    timer_t *tidptr;
    int s, j;
    sigset_t alarm_sig_set;
    siginfo_t alarm_sig_info;
    int expireCnt = 0;           /* Number of expirations of all timers */

    if (argc < 2)
        usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);

    tidlist = calloc(argc - 1, sizeof(timer_t));
    if (tidlist == NULL)
        errExit("malloc");

    sev.sigev_notify = SIGEV_SIGNAL;            /* Notify via signal */
    sev.sigev_signo = SIGALRM;
    sev.sigev_notify_attributes = NULL;
            /* Could be pointer to pthread_attr_t structure */

    int line_num = __LINE__ + 1;
    if (sigemptyset(&alarm_sig_set) == -1)
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigsetclear() failed"
            );

    line_num = __LINE__ + 1;
    if (sigaddset(&alarm_sig_set, SIGALRM) == -1)
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigaddset() failed"
            );

    line_num = __LINE__ + 1;
    if (sigprocmask(SIG_BLOCK, &alarm_sig_set, NULL) == -1)
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigprocmask() failed"
            );

    /* Create and start one timer for each command-line argument */

    for (j = 0; j < argc - 1; j++) {
        itimerspecFromStr(argv[j + 1], &ts);

        sev.sigev_value.sival_ptr = &tidlist[j];
                /* Passed as argument to threadFunc() */

        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1)
            errExit("timer_create");
        printf("Timer ID: %ld (%s)\n", (long) tidlist[j], argv[j + 1]);

        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1)
            errExit("timer_settime");
    }

    for (;;) {
        int sig_recv;

        line_num = __LINE__ + 1;
        if ((sig_recv = sigwaitinfo(&alarm_sig_set, &alarm_sig_info)) == -1)
            if (errno != EINTR)
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "sigwaitinfo() failed"
                    );

        tidptr = alarm_sig_info.si_value.sival_ptr;

        printf("[%s] Thread notify\n", currTime("%T"));
        printf("    timer ID=%ld\n", (long) *tidptr);
        printf("    timer_getoverrun()=%d\n", timer_getoverrun(*tidptr));

        expireCnt += 1 + timer_getoverrun(*tidptr);
        printf("main(): expireCnt = %d\n", expireCnt);
    }
}
