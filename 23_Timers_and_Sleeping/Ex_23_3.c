/* --------------------------------------------------------------------------
 * Exercise 23-3:
 * Write a program to show that if the evp argument to timer_create() is
 * specified as NULL, then this is equivalent to specifying evp as a pointer
 * to a sigevent structure with sigev_notify to SIGEV_SIGNAL, sigev_signo set
 * to SIGALRM and si_value.sival_int set to the timer ID.
 * -------------------------------------------------------------------------- */

#define _POSIX_C_SOURCE 199309
#include <errno.h>
#include <error.h>
#include <signal.h>           /* Definition of SIGEV_* constants */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

void handler(int sig) {
    printf("Signal %d caught\n", sig);
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    unsigned int line_num = 0;

    struct sigevent event, *evp;
    timer_t timer_id;

    struct itimerspec new_timer_spec, old_timer_spec;
    int flags = 0;

    if (argc > 1) {
        event.sigev_notify  = SIGEV_SIGNAL;
        event.sigev_signo   = SIGALRM;
        evp                 = &event;
    } else {
        evp                 = NULL;
    }

    sigset_t mask;

    line_num = __LINE__ + 1;
    if (sigemptyset(&mask) == -1) {
        error_at_line(1, errno, __FILE__, line_num, "sigemptyset() failed");
    }

    struct sigaction action;
    action.sa_handler   = handler;
    action.sa_flags     = 0;

    line_num = __LINE__ + 1;
    if (sigemptyset(&action.sa_mask) == -1) {
        error_at_line(1, errno, __FILE__, line_num, "sigemptyset() failed");
    }

    line_num = __LINE__ + 1;
    if (sigaction(SIGALRM, &action, NULL) == -1) {
        error_at_line(1, errno, __FILE__, line_num, "sigaction(SIGALRM) failed");
    }

    line_num = __LINE__ + 1;
    if (timer_create(CLOCK_REALTIME, evp, &timer_id) == -1) {
        error_at_line(1, errno, __FILE__, line_num, "timer_create() failed");
    }

    new_timer_spec.it_interval.tv_nsec  = 0;
    new_timer_spec.it_interval.tv_sec   = 0;
    new_timer_spec.it_value.tv_nsec     = 0;
    new_timer_spec.it_value.tv_sec      = 60;
    line_num = __LINE__ + 1;
    if (timer_settime(timer_id, flags, &new_timer_spec, &old_timer_spec) == -1) {
        error_at_line(1, errno, __FILE__, line_num, "timer_settime() failed");
    }

    sigset_t old_mask;

    line_num = __LINE__ + 1;
    if (sigprocmask(SIG_BLOCK, &mask, &old_mask) == -1) {
        error_at_line(1, errno, __FILE__, line_num, "sigprocmask() call failed");
    }

    line_num = __LINE__ + 1;
    if (sigsuspend(&old_mask) == -1) {
        if (errno != EINTR) {
            error_at_line(1, errno, __FILE__, line_num, "sigsuspend() failed");
        } else {
            printf("Signal caught\n");
        }
    }

    exit(EXIT_SUCCESS);
}