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
#include <getopt.h>
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
/* --------------------------------------------------------------------------
 * Main routine
 * -------------------------------------------------------------------------- */

int main(int argc, char* argv[]) {
    char *log_file_name         = NULL;

    /* Static variables used to parse passed arguments */

    static int show_help        = 0;
    static int verbose          = 0;
    static int use_real_abort   = 0;
    static int log_file_found   = 0;
    static int nodefer          = 0;

    static struct option long_options[] = {
        {"help",           no_argument,       &show_help,        1 },
        {"verbose",        no_argument,       &verbose,          1 },
        {"use-real-abort", no_argument,       &use_real_abort,   1 },
        {"log-file",       required_argument, &log_file_found,   1 },
        {0,                0,                 0,                 0 }
    };

    int option_index = 0;
    while (1) {
        int c = getopt_long_only(argc, argv, "", long_options, &option_index);
        if (c == -1) break;
        if (c != 0) {
            fprintf(stderr, "Argument parsing failed.\n");
            exit(EXIT_FAILURE);
        }
        if (verbose)
            fprintf(stderr,
                "c=%d; option_index=%d; optarg=%s\n",
                c,
                option_index,
                optarg);
        switch (option_index) {
            case 0: /* --help */
            case 1: /* --verbose */
            case 2: /* --use-real-abort */
                break;
            case 3: /* --log-file=<i> */
                if (optarg == NULL) {
                    fprintf(stderr, "No argument supplied for --log-file\n");
                    exit(EXIT_FAILURE);
                }
                log_file_name = optarg;
                break;
            default:
                fprintf(stderr, "Invalid arg found %d\n", option_index);
                break;
        }
    }

    if (!log_file_found) {
        fprintf(stderr, "No log file specified.\n");
        exit(EXIT_FAILURE);
    }

    fd = open(log_file_name,O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU|S_IRWXG);
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

    if (use_real_abort) {
        write(fd,"before call to abort()\n",24);
        abort();
        write(fd,"after call to abort()\n",23);
    } else {
        write(fd,"before call to my_abort()\n",27);
        my_abort();
        write(fd,"after call to my_abort()\n",26);
    }
    close(fd);
    exit(EXIT_SUCCESS);
}