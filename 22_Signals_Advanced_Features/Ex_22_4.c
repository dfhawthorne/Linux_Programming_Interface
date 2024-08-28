/* ==========================================================================
 * Ex 22-4:
 * Implement the System V functions sigset(), sighold(), sigrelse(), 
 * sigignore(), and sigpause() using the POSIX signal API.
 * ========================================================================== */

#include <error.h>
#include <errno.h>
#include <signal.h>
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

sighandler_t my_sigset(int sig, sighandler_t disp) {
    errno = ENOSYS;
    return SIG_ERR;
}

#define SIG_HOLD ((sighandler_t) 2)	/* Add signal to hold mask.  */

int my_sighold(int sig) {
    return (my_sigset(sig, SIG_HOLD) == SIG_ERR) ? -1 : 0;
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
    errno = ENOSYS;
    return -1;
}

/* ==========================================================================
 * Test suite
 * ========================================================================== */

void sig_handler(int sig) {
}

void test_sigset() {
    sighandler_t result;
    const unsigned int lineno = __LINE__ + 1;
    result = my_sigset(SIGUSR1, &sig_handler);
    if ((long int)result == -1L) {
        error_at_line(
            0,
            errno,
            __FILE__,
            lineno,
            "my_sigset failed"
            );
    }
}

void test_sighold() {
    const unsigned int lineno = __LINE__ + 1;
    int errnum = my_sighold(SIGUSR1);
    if (errnum == -1) {
        error_at_line(
            0,
            errno,
            __FILE__,
            lineno,
            "my_sighold failed"
            );
    }
}

void test_sigrelse() {
    const unsigned int lineno = __LINE__ + 1;
    int errnum = my_sigrelse(SIGUSR1);
    if (errnum == -1) {
        error_at_line(
            0,
            errno,
            __FILE__,
            lineno,
            "my_sigrelse failed"
            );
    }
}

void test_sigignore() {
    const unsigned int lineno = __LINE__ + 1;
    int errnum = my_sigignore(SIGUSR1);
    if (errnum == -1) {
        error_at_line(
            0,
            errno,
            __FILE__,
            lineno,
            "my_sigignore failed"
            );
    }
}

void test_sigpause() {
    const unsigned int lineno = __LINE__ + 1;
    int errnum = my_sigpause(SIGUSR1);
    if (errnum == -1) {
        error_at_line(
            0,
            errno,
            __FILE__,
            lineno,
            "my_sigpause failed"
            );
    }
}

int main(int argc, char *argv[]) {
    test_sigset();
    test_sighold();
    test_sigrelse();
    test_sigignore();
    test_sigpause();
}