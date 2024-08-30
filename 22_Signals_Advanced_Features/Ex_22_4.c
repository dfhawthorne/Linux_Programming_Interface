/* ==========================================================================
 * Ex 22-4:
 * Implement the System V functions sigset(), sighold(), sigrelse(), 
 * sigignore(), and sigpause() using the POSIX signal API.
 * ========================================================================== */

#define _POSIX_C_SOURCE 200809L

#include <error.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- 
NAME
       sigset, sighold, sigrelse, sigignore - System V signal API

SYNOPSIS
       #include <signal.h>

       typedef void (*sighandler_t)(int);

       sighandler_t sigset(int sig, sighandler_t disp);

       int sighold(int sig);

       int sigrelse(int sig);

       int sigignore(int sig);

   Feature Test Macro Requirements for glibc (see feature_test_macros(7)):

       sigset(), sighold(), sigrelse(), sigignore():
           _XOPEN_SOURCE >= 500

DESCRIPTION
       These  functions are provided in glibc as a compatibility interface for
       programs that make use of the historical System V signal API.  This API
       is  obsolete:  new applications should use the POSIX signal API (sigac‐
       tion(2), sigprocmask(2), etc.)

       The sigset() function modifies the disposition of the signal sig.   The
       disp  argument  can be the address of a signal handler function, or one
       of the following constants:

       SIG_DFL
              Reset the disposition of sig to the default.

       SIG_IGN
              Ignore sig.

       SIG_HOLD
              Add sig to the process's signal mask, but leave the  disposition
              of sig unchanged.

       If disp specifies the address of a signal handler, then sig is added to
       the process's signal mask during execution of the handler.

       If disp was specified as a value other than SIG_HOLD, then sig  is  re‐
       moved from the process's signal mask.

       The dispositions for SIGKILL and SIGSTOP cannot be changed.

       The sighold() function adds sig to the calling process's signal mask.

       The  sigrelse()  function removes sig from the calling process's signal
       mask.

       The sigignore() function sets the disposition of sig to SIG_IGN.

RETURN VALUE
       On success, sigset() returns SIG_HOLD if sig  was  blocked  before  the
       call, or the signal's previous disposition if it was not blocked before
       the call.  On error, sigset() returns -1, with errno  set  to  indicate
       the error.  (But see BUGS below.)

       The  sighold(),  sigrelse(), and sigignore() functions return 0 on suc‐
       cess; on error, these functions return -1 and set errno to indicate the
       error.

ERRORS
       For sigset() see the ERRORS under sigaction(2) and sigprocmask(2).

       For sighold() and sigrelse() see the ERRORS under sigprocmask(2).

       For sigignore(), see the errors under sigaction(2).
*/

/* My implementation of sigset() */

typedef void (*sighandler_t)(int);
#define SIG_HOLD ((sighandler_t) 2)	/* Add signal to hold mask.  */

sighandler_t my_sigset(int sig, sighandler_t disp) {
    static struct sigaction act, prev_act;

    unsigned int lineno = __LINE__ + 1;
    int errnum;

    act.sa_flags   = 0;

    if (sigemptyset(&act.sa_mask) == -1) {
        errnum = errno;
        error_at_line(0, errnum, __FILE__, lineno,
         "my_sigset failed while doing sigemptyset()");
        errno = errnum;
        return SIG_ERR;
    }

    lineno = __LINE__ + 1;
    if (sigaddset(&act.sa_mask, sig) == -1) {
        errnum = errno;
        error_at_line(0, errnum, __FILE__, lineno,
         "my_sigset failed while doing sigaddset()");
        errno = errnum;
        return SIG_ERR;
    }

    act.sa_handler = disp;

    if (sigaction(sig, &act, &prev_act) != 0) {
        return SIG_ERR;
    } else {
        return prev_act.sa_handler;
    }
}

int my_sighold(int sig) {
    unsigned int lineno;
    int errnum;
    sigset_t blockedMask;

    lineno = __LINE__ + 1;
    if (sigemptyset(&blockedMask) == -1) {
        errnum = errno;
        error_at_line(0, errno, __FILE__, lineno, 
            "my_sighold failed while calling sigemptyset()");
        errno = errnum;
        return -1;
    }

    lineno = __LINE__ + 1;
    if (sigaddset(&blockedMask, sig) == -1) {
        errnum = errno;
        error_at_line(EXIT_FAILURE, errno, __FILE__, lineno, 
            "my_sighold failed while calling sigaddset()");
        errno = errnum;
        return -1;
    }

    lineno = __LINE__ + 1;
    if (sigprocmask(SIG_BLOCK, &blockedMask, NULL) == -1) {
        errnum = errno;
        error_at_line(EXIT_FAILURE, errno, __FILE__, lineno, 
            "my_sighold failed while calling sigprocmask()");
        errno = errnum;
        return -1;
    }
    errno = 0;
    return 0;
}

int my_sigrelse(int sig) {
    return (my_sigset(sig, SIG_DFL) == SIG_ERR) ? -1 : 0;
}

int my_sigignore(int sig) {
    return (my_sigset(sig, SIG_IGN) == SIG_ERR) ? -1 : 0;
}

/* --------------------------------------------------------------------------
NAME
       sigpause - atomically release blocked signals and wait for interrupt

SYNOPSIS
       #include <signal.h>

       int sigpause(int sig);

DESCRIPTION
       Don't use this function.  Use sigsuspend(2) instead.

       The  function  sigpause()  is  designed  to  wait  for some signal.  It
       changes the process's signal mask (set of blocked  signals),  and  then
       waits  for  a signal to arrive.  Upon arrival of a signal, the original
       signal mask is restored.

RETURN VALUE
       If sigpause() returns, it was interrupted by a signal  and  the  return
       value is -1 with errno set to EINTR.
 * -------------------------------------------------------------------------- */

int my_sigpause(int sig) {
    unsigned int lineno;
    int errnum;
    static struct timespec time_out;
    time_out.tv_sec = 30;
    static sigset_t sig_set;

    lineno = __LINE__ + 1;
    if (sigemptyset(&sig_set) == -1) {
        errnum = errno;
        error_at_line(0, errnum, __FILE__, lineno,
         "my_sigpause failed while doing sigemptyset()");
        errno = errnum;
        return -1;
    }

    lineno = __LINE__ + 1;
    if (sigaddset(&sig_set, sig) == -1) {
        errnum = errno;
        error_at_line(0, errnum, __FILE__, lineno,
         "my_sigpause failed while doing sigaddset()");
        errno = errnum;
        return -1;
    }

    lineno = __LINE__ + 1;
    int captured_sig = sigtimedwait(&sig_set, NULL, &time_out);

    if (captured_sig == -1) {
        return -1;
    } 
    if (captured_sig == sig) {
        errno = EINTR;
        return -1;
    } else {
        errnum = EINVAL;
        error_at_line(0, errnum, __FILE__, lineno,
         "incorrect signal (%d) returned - expected %d", captured_sig, sig);
        errno = errnum;
        return -1;
    }
}

/* ==========================================================================
 * Test suite
 * ========================================================================== */

void sig_handler(int sig) {
    printf("Signal (%d) caught by handler\n", sig);
    fflush(stdout);
}

void test_sigset() {
    sighandler_t result;
    unsigned int lineno = __LINE__ + 1;
    result = my_sigset(SIGUSR1, &sig_handler);
    if (result == SIG_ERR) {
        error_at_line(0, errno, __FILE__, lineno, "my_sigset failed");
    } else {
        sighandler_t restore_result;
        lineno = __LINE__ + 1;
        restore_result = my_sigset(SIGUSR1, result);
        if (restore_result == SIG_ERR) {
            error_at_line(0, errno, __FILE__, lineno, "my_sigset failed");
        } else {
            printf("my_sigset passed\n");
        }
    }
}

void test_sighold() {
    const unsigned int lineno = __LINE__ + 1;
    int errnum = my_sighold(SIGUSR1);
    if (errnum == -1) {
        error_at_line(0, errno, __FILE__, lineno, "my_sighold failed");
    } else {
        printf("my_sighold passed\n");
    }
}

void test_sigrelse() {
    const unsigned int lineno = __LINE__ + 1;
    int errnum = my_sigrelse(SIGUSR1);
    if (errnum == -1) {
        error_at_line(0, errno, __FILE__, lineno, "my_sigrelse failed");
    } else {
        printf("my_sigrelse passed\n");
    }
}

void test_sigignore() {
    const unsigned int lineno = __LINE__ + 1;
    int errnum = my_sigignore(SIGUSR1);
    if (errnum == -1) {
        error_at_line(0, errno, __FILE__, lineno, "my_sigignore failed");
    } else {
        printf("my_sigignore passed\n");
    }
}

void test_sigpause() {
    unsigned int lineno;
    int errnum;
    sigset_t blockedMask;

    lineno = __LINE__ + 1;
    if (sigemptyset(&blockedMask) == -1) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, lineno, 
            "my_sigpause failed while calling sigemptyset()");
    }

    lineno = __LINE__ + 1;
    if (sigaddset(&blockedMask, SIGUSR2) == -1) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, lineno, 
            "my_sigpause failed while calling sigaddset()");
    }

    lineno = __LINE__ + 1;
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, lineno, 
            "my_sigpause failed while calling sigprocmask()");
    }
    
    lineno = __LINE__ + 1;
    errnum = my_sigpause(SIGUSR2);
    if (errnum == -1) {
        if (errno != EINTR) {
            error_at_line(0, errno, __FILE__, lineno, "my_sigpause failed");
        } else {
            printf("my_sigpause passed\n");
        }
    } else {
        error_at_line(0, EINVAL, __FILE__, lineno, "my_sigpause failed");
    }
}

int main(int argc, char *argv[]) {
    test_sigset();
    test_sighold();
    test_sigignore();
    test_sigrelse();
    test_sigpause();
    printf("*** All tests done ***\n");
    fflush(stdout);
    exit(EXIT_SUCCESS);
}