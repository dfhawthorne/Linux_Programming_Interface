/* --------------------------------------------------------------------------
 * Ex 23-1:
 * Although alarm() is implemented as a system call within the Linux kernel,
 * this is redundant. Implement alarm() using setitimer().
 * -------------------------------------------------------------------------- */

#define _GNU_SOURCE
#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- 
 * NAME
 *      alarm - set an alarm clock for delivery of a signal
 *
 * LIBRARY
 *      Standard C library (libc, -lc)
 * 
 * SYNOPSIS
 *      #include <unistd.h>
 *
 *      unsigned int alarm(unsigned int seconds);
 *
 * DESCRIPTION
 *      alarm()  arranges  for  a SIGALRM signal to be delivered to the calling
 *      process in seconds seconds.
 *
 *      If seconds is zero, any pending alarm is canceled.
 *
 *      In any event any previously set alarm() is canceled.
 *
 * RETURN VALUE
 *      alarm() returns the number of seconds remaining  until  any  previously
 *      scheduled alarm was due to be delivered, or zero if there was no previ‐
 *      ously scheduled alarm.
 * -------------------------------------------------------------------------- */

unsigned int my_alarm(unsigned int seconds) {
    static struct itimerval new_timer_val;
    
    new_timer_val.it_value.tv_sec       = seconds;
    new_timer_val.it_value.tv_usec      = 0;
    new_timer_val.it_interval.tv_sec    = 0;
    new_timer_val.it_interval.tv_usec   = 0;
    
    return setitimer(ITIMER_REAL, &new_timer_val, NULL);
}

void alarm_handler(int signum) {
    printf("Alarm signal received!\n");
}

/* -------------------------------------------------------------------------- 
 * Main routine to test my_alarm()
 * -------------------------------------------------------------------------- */

int main(int argc, char *argv[]) {
    unsigned int linenum;
    struct sigaction sa;

    sa.sa_handler = alarm_handler;
    sa.sa_flags = 0;

    linenum = __LINE__ + 1;    
    if (sigemptyset(&sa.sa_mask) == -1) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, linenum, "Call to sigemptyset() failed");
    }

    linenum = __LINE__ + 1;    
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, linenum, "Call to sigaction() failed");
    }
    
    linenum = __LINE__ + 1;    
    if (my_alarm(1) == -1) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, linenum, "Call to my_alarm() failed");
    }
    
    linenum = __LINE__ + 1;    
    if (pause() == -1) {
        if (errno != EINTR) {
            error_at_line(EXIT_FAILURE, errno, __FILE__, linenum, "Call to pause() failed");
        } else {
            printf("Alarm succeeded\n");
        }
    }
    
    exit(EXIT_SUCCESS);
}