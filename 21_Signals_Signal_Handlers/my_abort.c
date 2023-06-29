/* --------------------------------------------------------------------------
 * Exercise 21-1: Implement abort()
 *
 * NAME
 *      abort - cause abnormal process termination
 *
 * SYNOPSIS
 *      #include <stdlib.h>
 *
 *      void abort(void);
 *
 * DESCRIPTION
 *      The abort() function first unblocks the SIGABRT signal, and then
 *      raises that signal for the calling process (as though raise(3)
 *      was called).  This results in the abnormal termination of the process
 *      unless the SIGABRT signal is caught and the signal handler does not
 *      return (see longjmp(3)).
 *
 *      If the SIGABRT signal is ignored, or caught by a handler that returns,
 *      the abort() function will still terminate the process.  It does this
 *      by restoring the default disposition for SIGABRT and then raising the
 *      signal for a second time.
 *
 * RETURN VALUE
 *      The abort() function never returns.
 * -------------------------------------------------------------------------- */

#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- 
 * Global variables
 * -------------------------------------------------------------------------- */

static int fd = 0;

/* --------------------------------------------------------------------------
 * My implementation of abort()
 * -------------------------------------------------------------------------- */

void my_abort(void) {
    static struct sigaction oldact;
    pid_t my_pid = getpid();

    if (sigaction(SIGABRT, NULL, &oldact) == -1) _exit(1);

    void (*abort_handler)(int) = oldact.sa_handler; /* Get current signal handler */
    oldact.sa_handler = SIG_DFL; /* Restore default behaviour */
    sigemptyset(&oldact.sa_mask); /* Unblock all signals */
    if (sigaction(SIGABRT, &oldact, NULL) == -1) _exit(1);

    write(fd,"before call to signal handler.\n", 32);
    if (abort_handler != SIG_DFL && abort_handler != SIG_IGN) {
        (*abort_handler)(SIGABRT);
    } else {
        write(fd,"no signal handler found.\n",26);
        close(fd);
    }
    kill(my_pid,SIGABRT);
}

/* --------------------------------------------------------------------------
 * Signal handler
 * -------------------------------------------------------------------------- */

void sig_handler(int sig) {
    write(fd,"Signal caught\n",15);
    close(fd);
}

int main(int argc, char *argv[]) {
    fd = open("test.log",O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU|S_IRWXG);
    if (fd == -1) {
        fprintf(stderr, "Unable to create 'test.log': %m\n");
        exit(EXIT_FAILURE);
    }

    static struct sigaction act;
    act.sa_handler = sig_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGABRT,&act,NULL) == -1) {
        fprintf(stderr, "addition of signal handler failed: %m\n");
        exit(EXIT_FAILURE);
    }
    write(fd,"signal handler installed for SIGABRT.\n",39);

    my_abort();
    exit(EXIT_SUCCESS);
}