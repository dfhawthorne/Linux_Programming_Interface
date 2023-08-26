/* --------------------------------------------------------------------------
 * Exercise 22-1:
 * Section 22.2 noted that if a stopped process that has established a
 * handler for and blocked SIGCONT is later resumed as a consequence of
 * receiving a SIGCONT, then the handler is invoked only when SIGCONT is
 * unblocked. 
 * -------------------------------------------------------------------------- */

#define TRUE 1
#define FALSE 0
#define DATE_BUFFER_SIZE 20
#define _GNU_SOURCE
#define _POSIX_C_SOURCE
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* --------------------------------------------------------------------------
 * Global variables
 * -------------------------------------------------------------------------- */

static int log_fd = 0;
static char *pgm_name_str = NULL;
static int pgm_name_len = 0;

/* --------------------------------------------------------------------------
 * Signal handler
 * -------------------------------------------------------------------------- */

void sig_handler(int sig) {
    time_t sig_now;
    struct tm sig_now_tm;
    char sig_time_str[DATE_BUFFER_SIZE];
    
    write(log_fd, pgm_name_str, pgm_name_len);
    time(&sig_now);
    localtime_r(&sig_now, &sig_now_tm);
    size_t num_bytes = strftime(
        sig_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &sig_now_tm);
    if (num_bytes > 0) {
        write(log_fd, " ", 1);
        write(log_fd, sig_time_str, num_bytes);
    }
    write(log_fd, ": Signal caught\n", 16);
}

/* --------------------------------------------------------------------------
 * Main routine
 * -------------------------------------------------------------------------- */

int main(int argc, char* argv[]) {
    char *log_file_name = NULL;

    /* Static variables used to parse passed arguments */

    static int show_help          = FALSE,
               verbose            = FALSE,
               log_file_found     = FALSE,
               idle_for_found     = FALSE,
               mask_interrupt     = FALSE;
    static struct option long_options[] = {
        {"help",         no_argument,       &show_help,          TRUE},
        {"verbose",      no_argument,       &verbose,            TRUE},
        {"log-file",     required_argument, &log_file_found,     TRUE},
        {"idle-for",     required_argument, &idle_for_found,     TRUE},
        {"mask",         no_argument,       &mask_interrupt,     TRUE},
        {0,              0,                 0,                   0}
    };

    int option_index = 0;
    long idle_for = 100;
    pgm_name_str = argv[0];
    pgm_name_len = strlen(pgm_name_str);

    while (1) {
        int c = getopt_long_only(argc, argv, "", long_options, &option_index);
        if (c == -1) break;
        if (verbose)
            fprintf(stderr,
                "%s: c=%d; option_index=%d; optarg=%s\n",
                pgm_name_str,
                c,
                option_index,
                optarg);
        switch (option_index) {
            case 0: /* --help */
            case 1: /* --verbose */
            case 4: /* --mask */
                break;
            case 2: /* --log-file=<i> */
                if (optarg == NULL) {
                    fprintf(stderr,
                        "%s: No argument supplied for --log-file\n",
                        pgm_name_str);
                    exit(EXIT_FAILURE);
                }
                log_file_name = optarg;
                break;
            case 3: /* --idle-for=<i> */
                if (optarg == NULL) {
                    fprintf(stderr,
                        "%s: No argument supplied for --idle-for\n",
                        pgm_name_str);
                    exit(EXIT_FAILURE);
                }
                idle_for = strtol(optarg, NULL, 0);
                if (idle_for <= 0) {
                    fprintf(stderr,
                        "%s: Invalid argument (%s) for --idle-for\n",
                        pgm_name_str,
                        optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Invalid arg found %d\n", option_index);
                break;
        }
    }

    /* Show the HELP message and EXIT. */

    if (show_help) {
        fprintf(stderr,
            "%s [--help] [--verbose] [--idle-for=n] --log-file=f\n\
\t--help\n\t\tshow this help message and exit.\n\
\t--verbose\n\t\tshow verbose messages\n\
\t--idle-for=n\n\t\tidle program for <n> seconds.\n\
\t\tDefault is 100 seconds.\n\
\t--log-file=f\n\t\twrite progress information to file name, <f>.\n\
\t\tRequired parameter.\n\
\t--mask\n\t\tmask SIGCONT interrupt while idling\n",
            pgm_name_str);
            exit(EXIT_SUCCESS);
    }

    /* Open log file to allow synchronous logging across multiple threads */

    if (!log_file_found) {
        fprintf(stderr, "%s: No log file specified.\n", pgm_name_str);
        exit(EXIT_FAILURE);
    }

    log_fd = open(log_file_name, O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU|S_IRWXG);
    if (log_fd == -1) {
        fprintf(stderr,
            "%s: Unable to open log file ('%s'): %m\n",
            pgm_name_str,
            log_file_name);
        exit(EXIT_FAILURE);
    }
    if (verbose)
        fprintf(stderr,
            "%s: Log file ('%s') opened.\n",
            pgm_name_str,
            log_file_name);

    /* Block handling of SIGCONT interrupts */

    sigset_t block_set, prev_mask;

    if (mask_interrupt) {
        if (verbose)
            fprintf(stderr,
                "%s: Disable SIGCONT interrupts from being handled.\n",
                pgm_name_str);

        sigemptyset(&block_set);
        if (sigaddset(&block_set, SIGCONT) == -1) {
            fprintf(stderr,
                "%s: failed to set process mask for SIGCONT: %m\n",
                pgm_name_str);
            exit(EXIT_FAILURE);
        }
        if (sigprocmask(SIG_BLOCK, &block_set, &prev_mask) == -1) {
            fprintf(stderr,
                "%s: Unable to add mask for SIGCONT: %m\n",
                pgm_name_str);
            exit(EXIT_FAILURE);
        }
    } else {
        if (verbose)
            fprintf(stderr,
                "%s: No need to disable SIGCONT interrupts as --mask was not passed.\n",
                pgm_name_str);
    }

    /* Set signal handler for SIGCONT */

    static struct sigaction act;
    act.sa_handler = sig_handler;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGCONT, &act, NULL) == -1) {
        fprintf(stderr,
            "%s: Addition of signal handler for SIGCONT failed: %m\n",
            pgm_name_str);
        exit(EXIT_FAILURE);
    }
    if (verbose)
        fprintf(stderr,
            "%s: Signal handler added for SIGCONT.\n",
            pgm_name_str);

    /* Display log message before idling */

    time_t now;
    struct tm now_tm;
    char current_time_str[DATE_BUFFER_SIZE];

    time(&now);
    localtime_r(&now, &now_tm);
    size_t num_bytes = strftime(
        current_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &now_tm);
    
    write(log_fd, pgm_name_str, pgm_name_len);
    if (num_bytes > 0) {
        write(log_fd, " ", 1);
        write(log_fd, current_time_str, num_bytes);
    }
    write(log_fd, ": Signal handler added - idling started\n", 40);
    if (verbose)
        fprintf(stderr,
            "%s: Started idling for %ld seconds.\n",
            pgm_name_str,
            idle_for);
    for (long i = 0; i < idle_for; i++) {
        sleep(1);
    }

    /* Idling done */

    time(&now);
    localtime_r(&now, &now_tm);
    num_bytes = strftime(
        current_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &now_tm);
    
    write(log_fd, pgm_name_str, pgm_name_len);
    if (num_bytes > 0) {
        write(log_fd, " ", 1);
        write(log_fd, current_time_str, num_bytes);
    }
    write(log_fd, ": Idling ended\n", 15);
    if (verbose)
        fprintf(stderr,
            "%s: Idling ended after %ld seconds.\n",
            pgm_name_str,
            idle_for);

    /* Enable SIGCONT interrupts to be handled */

    if (mask_interrupt) {
        if (verbose)
            fprintf(stderr,
                "%s: Enable SIGCONT interrupts to be handled.\n",
                pgm_name_str);

        if (sigprocmask(SIG_UNBLOCK, &block_set, &prev_mask) == -1) {
            fprintf(stderr,
                "%s: Unable to remove mask for SIGCONT: %m\n",
                pgm_name_str);
            exit(EXIT_FAILURE);
        }
    } else {
        if (verbose)
            fprintf(stderr,
                "%s: No need to enable SIGCONT interrupts as they were not masked.\n",
                pgm_name_str);
    }

    /* Sleep for one (1) second to allow for signals to be processed */

    sleep(1);

    /* Close log file. */

    close(log_fd);
    if (verbose)
        fprintf(stderr,
            "%s: Log file ('%s') closed.\n",
            pgm_name_str,
            log_file_name);
}