/* --------------------------------------------------------------------------
 * Exercise 20-4: Implement siginterrupt() using sigaction().
 *
 * NAME
 *        siginterrupt - allow signals to interrupt system calls
 *
 * SYNOPSIS
 *        #include <signal.h>
 *
 *        int siginterrupt(int sig, int flag);
 *
 *    Feature Test Macro Requirements for glibc (see feature_test_macros(7)):
 *
 *        siginterrupt():
 *            _XOPEN_SOURCE >= 500
 *                ||  Since glibc 2.12:  _POSIX_C_SOURCE >= 200809L
 *                ||  Glibc versions <= 2.19:  _BSD_SOURCE
 *
 * DESCRIPTION
 *       The siginterrupt() function changes the restart behavior when a
 *       system call is interrupted by the signal sig.  If the flag argument
 *       is false (0), then system calls will be restarted if interrupted by
 *       the specified signal sig.  This is the default behavior in Linux.
 *
 *       If the flag argument is true (1) and no data has been transferred,
 *       then a system call interrupted by the signal sig will return -1 and
 *       errno will be set to EINTR.
 *
 *       If the flag argument is true (1) and data transfer has started, then
 *       the system call will be interrupted and will return the actual amount
 *       of data transferred.
 *
 * RETURN VALUE
 *       The siginterrupt() function returns 0 on success.  It returns -1 if
 *       the signal number sig is invalid, with errno set to indicate the
 *       cause of the error.
 *
 * ERRORS
 *       EINVAL The specified signal number is invalid.
 *
 * -------------------------------------------------------------------------- */

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/* --------------------------------------------------------------------------
 * My implementation of siginterrupt
 * -------------------------------------------------------------------------- */

int my_siginterrupt(int sig, int flag) {
    static struct sigaction oldact;
    if (sigaction(sig, NULL, &oldact) == -1) return -1;
    oldact.sa_flags |= SA_RESTART;
    if (flag) oldact.sa_flags ^= SA_RESTART;
    return (sigaction(sig, &oldact, NULL));
}

/* --------------------------------------------------------------------------
 * Signal handler
 * -------------------------------------------------------------------------- */

volatile sig_atomic_t sigint_received = 0;
volatile sig_atomic_t sigusr1_received = 0;

static void signal_handler(int signal) {
    switch (signal) {
        case SIGINT:
            sigint_received = 1;
            break;
        case SIGUSR1:
            sigusr1_received = 1;
            break;
        default:
            break;
    }
}   /* signal_handler */

int main(int argc, char *argv[]) {
    char *full_pgm_name  = strdup(argv[0]);
    if (full_pgm_name == NULL) {
        fprintf(stderr,
            "%s: unable to duplicate program name: %m\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }
    char *pgm_name       = basename(full_pgm_name);
    if (pgm_name == NULL) {
        fprintf(stderr,
            "%s: Unable to extract base name for program: %m\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }
    
    static struct sigaction new_signal_action;
    pid_t my_pid = getpid();

    printf("%s: Enable signal handler for SIGUSR1\n", pgm_name);
    new_signal_action.sa_flags   = 0;
    sigemptyset(&new_signal_action.sa_mask);

    new_signal_action.sa_handler = signal_handler;
    if (sigaction(SIGUSR1, &new_signal_action, NULL) == -1) {
        fprintf(stderr,
            "%s: adding of signal handler (signal_handler) for signal=%d through sigaction() failed: %m",
            pgm_name,
            10);
        exit(EXIT_FAILURE);
    }

    printf("%s: Send SIGUSR1 to self\n", pgm_name);
    if (kill(my_pid, SIGUSR1) == -1) {
        fprintf(stderr, "%s: send of SIGUSR1 to self failed: %m\n", pgm_name);
        exit(EXIT_FAILURE);
    }

    printf("%s: signal was %sreceived.\n", pgm_name, (sigusr1_received) ? "" : "not ");

    printf("%s: setting of signal handler (signal_handler) through siginterrupt(%d,0)\n",
            pgm_name,
            10);
    if (my_siginterrupt(SIGUSR1,0) == -1) {
        fprintf(stderr,
            "%s: setting of signal handler (signal_handler) through siginterrupt(%d,0) failed: %m",
            pgm_name,
            10);
        exit(EXIT_FAILURE);
    }

    sigusr1_received = 0;
    printf("%s: Send SIGUSR1 to self\n", pgm_name);
    if (kill(my_pid, SIGUSR1) == -1) {
        fprintf(stderr, "%s: send of SIGUSR1 to self failed: %m\n", pgm_name);
        exit(EXIT_FAILURE);
    }

    printf("%s: signal was %sreceived.\n", pgm_name, (sigusr1_received) ? "" : "not ");

    printf("%s: setting of signal handler (signal_handler) through siginterrupt(%d,1)\n",
            pgm_name,
            10);
    if (my_siginterrupt(SIGUSR1,1) == -1) {
        fprintf(stderr,
            "%s: setting of signal handler (signal_handler) through siginterrupt(%d,1) failed: %m\n",
            pgm_name,
            10);
        exit(EXIT_FAILURE);
    }

    sigusr1_received = 0;
    printf("%s: Send SIGUSR1 to self\n", pgm_name);
    if (kill(my_pid, SIGUSR1) == -1) {
        fprintf(stderr, "%s: send of SIGUSR1 to self failed: %m\n", pgm_name);
        exit(EXIT_FAILURE);
    }

    printf("%s: signal was %sreceived.\n", pgm_name, (sigusr1_received) ? "" : "not ");

    printf("%s: Exit.\n", pgm_name);

    exit(EXIT_SUCCESS);
}