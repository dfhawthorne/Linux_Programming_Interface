/* ==========================================================================
 * Ex 22-3:
 * Section 22.10 stated that accepting signals using sigwaitinfo() is faster
 * than the use of a signal handler plus sigsuspend(). The signals/
 * sig_speed_sigsuspend.c, supplied in the source code distribution for this
 * book, uses sigsuspend() to alternately send signals back and forward
 * between a parent and a child process. Time the operation of this program
 * to exchange one million signals between the two processes. (The number of
 * signals to exchange is provided as a command-line argument to the program.)
 * Create a modified version of the program that instead uses sigwaitinfo(),
 * and time that version. What is the speed difference between the two
 * programs.
 * ========================================================================== */


/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* sig_speed_sigsuspend.c

   This program times how fast signals are sent and received.

   The program forks to create a parent and a child process that alternately
   send signals to each other (the child starts first). Each process catches
   the signal with a handler, and waits for signals to arrive using
   sigsuspend().

   Usage: $ time ./sig_speed_sigsuspend num-sigs

   The 'num-sigs' argument specifies how many times the parent and
   child send signals to each other.

   Child                                  Parent

   for (s = 0; s < num_sigs; s++) {        for (s = 0; s < num_sigs; s++) {
       send signal to parent                  wait for signal from child
       wait for a signal from parent          send a signal to child
   }                                      }
*/

#define _GNU_SOURCE
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

static void
handler(int sig)
{
}

FILE *log_file = NULL;

#define TESTSIG SIGUSR1

void print_stats(const char *msg, long num_iterations) {
    /*------------------------------------------------------------------------*\
    | Print statistics for later analysis                                      |
    |  1. Message                                                              |
    |  2. Number of iterations                                                 |
    |  3. Wall clock time in seconds                                           |
    |  4. User time in seconds                                                 |
    |  5. System time in seconds                                               |
    \*------------------------------------------------------------------------*/
    
    struct tms t;
    clock_t clockTime = clock();
    long clockTicks = sysconf(_SC_CLK_TCK);  /* Clocks per tick for conversion */
    
    if (clockTicks == -1)
    {
        perror("sysconf(_SC_CLK_TCK) failed");
        exit(EXIT_FAILURE);
    }
    if (clockTime  == -1)
    {
        perror("clock() failed");
        exit(EXIT_FAILURE);
    }
    if (times(&t)  == -1)
    {
        perror("times() failed");
        exit(EXIT_FAILURE);
    }
    fprintf(log_file,
        "%s\t%ld\t%.2f\t%.2f\t%.2f\n",
        msg,
        num_iterations,
        (double) clockTime / CLOCKS_PER_SEC,
        (double) t.tms_utime / clockTicks,
        (double) t.tms_stime / clockTicks
        );
}

int
main(int argc, char *argv[])
{
    static int
        do_help        = 0,
        verbose        = 0,
        found_num_sigs = 0,
        found_log_file = 0,
        use_sigsuspend = 0;

    static struct option options[] = {
        {"help",       no_argument,       &do_help,        1},
        {"verbose",    no_argument,       &verbose,        1},
        {"num-sigs",   required_argument, &found_num_sigs, 1},
        {"sigsuspend", no_argument,       &use_sigsuspend, 1},
        {"log-file",   required_argument, &found_log_file, 1},
        {NULL,         0,                 NULL,            0}
    };

    static int long_index   = 0;
    long int num_sigs       = 0L;
    char *log_file_name     = NULL;
    
    while (1) {
        int c = getopt_long_only(argc, argv, "", options, &long_index);
        if (c == -1) break;
        switch (long_index) {
            case 0: /* help */
            case 1: /* verbose */
            case 3: /* sigsuspend */
                break;
            case 2: /* num_sigs= */
                num_sigs = atol(optarg);
                break;
            case 4: /* log-file= */
                log_file_name = optarg;
                break;
        }
    }

    if (found_log_file) {
        log_file = fopen(log_file_name, "a");
    } else {
        log_file = stdout;
    }

    if (verbose) {
        fprintf(log_file,
            "%s: help=%s, verbose=True, sigsuspend=%s. num_sigs=%'ld\n",
            argv[0],
            (do_help) ? "True" : "False",
            (use_sigsuspend) ? "True" : "False",
            num_sigs);
        fflush(log_file);
    }

    if (use_sigsuspend) {
        if (verbose) {
            fprintf(log_file,
                "%s: Set up signal handler\n",
                argv[0]);
            fflush(log_file);
        }
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handler;
        if (sigaction(TESTSIG, &sa, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
    }

    /* Block the signal before fork(), so that the child doesn't manage
    to send it to the parent before the parent is ready to catch it */

    if (verbose) {
        fprintf(log_file,
            "%s: Block signal before fork\n",
            argv[0]);
        fflush(log_file);
    }
    sigset_t blockedMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    if (verbose) {
        fprintf(log_file,
            "%s: Create empty signal mask\n",
            argv[0]);
        fflush(log_file);
    }
    sigset_t emptyMask;
    sigemptyset(&emptyMask);

    if (verbose) {
        fprintf(log_file,
            "%s: Do fork\n",
            argv[0]);
        fflush(log_file);
    }
    pid_t childPid = fork();
    switch (childPid) {
    case -1: perror("fork");
        exit(EXIT_FAILURE);

    case 0:     /* child */
        for (int scnt = 0; scnt < num_sigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            if (use_sigsuspend) {
                if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
                        perror("sigsuspend");
                        exit(EXIT_FAILURE);
                }
            } else {
                if (sigwaitinfo(&emptyMask, NULL) == -1) {
                    perror("sigwaitinfo");
                    exit(EXIT_FAILURE);
                }
            }
        }
        exit(EXIT_SUCCESS);

    default: /* parent */
        for (int scnt = 0; scnt < num_sigs; scnt++) {
            if (use_sigsuspend) {
                if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
                        perror("sigsuspend");
                        exit(EXIT_FAILURE);
                }
            } else {
            }
            if (kill(childPid, TESTSIG) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
        }
        if (use_sigsuspend) {
            print_stats("test using sigsuspend completed", num_sigs);
        } else {
            print_stats("test using sigwaitinfo completed", num_sigs);
        }
        exit(EXIT_SUCCESS);
    }
}
