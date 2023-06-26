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
#include <stdlib.h>
#include <sys/types.h>

/* --------------------------------------------------------------------------
 * My implementation of siginterrupt
 * -------------------------------------------------------------------------- */

int my_siginterrupt(int sig, int flag) {
    struct sigaction * oldact = NULL;
    if (sigaction(sig, NULL, oldact) == -1) return -1;
    oldact -> sa_flags |= SA_RESTART;
    return (sigaction(sig, oldact, NULL));
}

int main(int argc, char *argv[]) {
    exit(EXIT_SUCCESS);
}