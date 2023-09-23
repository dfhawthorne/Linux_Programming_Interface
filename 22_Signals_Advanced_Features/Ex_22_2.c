/* --------------------------------------------------------------------------
 * Ex 23-2:
 * If both a realtime and a standard signal are pending for a process, SUSv3
 * leaves it unspecified which is delivered first. Write a program that shows
 * what Linux does in this case.
 * -------------------------------------------------------------------------- */

#define TRUE 1
#define FALSE 0
#define DATE_BUFFER_SIZE 20
#define MSG_BUFFER_SIZE 128

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
static char *all_signal_name[NSIG+1];

/* --------------------------------------------------------------------------
 * Signal handler
 * -------------------------------------------------------------------------- */

void sig_handler(int sig) {
    const char status_start_msg[] = ": Signal caught (";
    const char status_end_msg[] = ")\n";
    time_t sig_now;
    struct tm sig_now_tm;
    char sig_time_str[DATE_BUFFER_SIZE],
         msg_buffer[MSG_BUFFER_SIZE+1];
    size_t msg_len = 0;
    
    msg_buffer[0] = '\0';
    char *signal_name;
    char signal_num[6];
    if (sig < 1 && sig > NSIG || all_signal_name[sig] == NULL) {
        snprintf(signal_num, 6, "%d", sig);
        signal_name = signal_num;
    } else {
        signal_name = all_signal_name[sig];
    }

    if (pgm_name_len < MSG_BUFFER_SIZE) 
        strncat(msg_buffer, pgm_name_str, pgm_name_len);
    time(&sig_now);
    localtime_r(&sig_now, &sig_now_tm);
    size_t num_bytes = strftime(
        sig_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &sig_now_tm);
    if (num_bytes > 0 && strlen(msg_buffer) + num_bytes + 1 < MSG_BUFFER_SIZE) {
        strcat(msg_buffer, " ");
        strncat(msg_buffer, sig_time_str, num_bytes);
    }

    if (strlen(msg_buffer) + strlen(status_start_msg) < MSG_BUFFER_SIZE)
        strcat(msg_buffer, status_start_msg);
    if (strlen(msg_buffer) + strlen(signal_name) < MSG_BUFFER_SIZE)
        strcat(msg_buffer, signal_name);
    if (strlen(msg_buffer) + strlen(status_end_msg) < MSG_BUFFER_SIZE)
        strcat(msg_buffer, status_end_msg);
    msg_len = strlen(msg_buffer);
    if (msg_len > 0)
        write(log_fd, msg_buffer, msg_len);
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
               idle_for_found     = FALSE;
    static struct option long_options[] = {
        {"help",         no_argument,       &show_help,          TRUE},
        {"verbose",      no_argument,       &verbose,            TRUE},
        {"log-file",     required_argument, &log_file_found,     TRUE},
        {"idle-for",     required_argument, &idle_for_found,     TRUE},
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
\t\tRequired parameter.\n",
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

    /* Log status messages */

    const char
        msg_sig_blocked[]           = ": All signals blocked\n",
        msg_sig_hndlr_added_start[] = ": Signal handler added for ",
        msg_sig_hndlr_added_end[]   = " \n",
        msg_idle_started[]          = ": Idling started\n",
        msg_idle_ended[]            = ": Idling ended\n",
        msg_sig_unblocked[]         = ": All signal unblocked\n";
    char msg_buffer[MSG_BUFFER_SIZE];
    int  msg_len = 0;

    time_t now;
    struct tm now_tm;
    char current_time_str[DATE_BUFFER_SIZE];
    size_t num_bytes = 0;

    /* Block handling of interrupts */

    sigset_t block_set, prev_mask;

    if (verbose)
        fprintf(stderr,
            "%s: Disable all interrupts from being handled.\n",
            pgm_name_str);

    sigfillset(&block_set);
    if (sigprocmask(SIG_BLOCK, &block_set, &prev_mask) == -1) {
        fprintf(stderr,
            "%s: Unable to add mask for all signals: %m\n",
            pgm_name_str);
        exit(EXIT_FAILURE);
    }

    /* Display log message about blocking of signals */

    time(&now);
    localtime_r(&now, &now_tm);
    num_bytes = strftime(
        current_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &now_tm);
    
    msg_buffer[0] = '\0';
    if (pgm_name_len < MSG_BUFFER_SIZE)
        strncat(msg_buffer, pgm_name_str, pgm_name_len);
    if (num_bytes > 0 && strlen(msg_buffer) + num_bytes + 1 < MSG_BUFFER_SIZE) {
        strcat(msg_buffer, " ");
        strncat(msg_buffer, current_time_str, num_bytes);
    }
    if (strlen(msg_buffer) + strlen(msg_sig_blocked) < MSG_BUFFER_SIZE)
        strcat(msg_buffer, msg_sig_blocked);
    msg_len = strlen(msg_buffer);
    if (msg_len > 0)
        write(log_fd, msg_buffer, msg_len);

    /* Build global array of signal names */

    if (verbose)
        fprintf(stderr,
            "%s: Populating global array of signal names\n",
            pgm_name_str);
    for (int sig = 1; sig < NSIG; sig++) {
        if (verbose)
            fprintf(stderr,
                "%s: generating signal name for signal=%d\n",
                pgm_name_str,
                sig);
        if (sig < 32) { /* Avoids segmentation faults for higher signal numbers */
            char *signal_name = strdup(sigdescr_np(sig));
            if (signal_name) {
                all_signal_name[sig] = strdup(signal_name);
                free(signal_name);
                continue;
            }
        }        
        if (verbose)
            fprintf(stderr,
                "%s: no signal name found for signal=%d\n",
                pgm_name_str,
                sig);
        char signal_num[6];
        snprintf(signal_num, 5, "%d", sig);
        signal_num[5] = '\0';
        all_signal_name[sig] = strdup(signal_num);
    }
    if (verbose)
        fprintf(stderr,
            "%s: Global array of signal names populated\n",
            pgm_name_str);

    /* Set signal handlers for all signals */

    static struct sigaction act;
    act.sa_handler = sig_handler;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);
    for (int sig = 1; sig < NSIG; sig++) {
        switch (sig) {
            case  9: /* SIGKILL */
            case 11: /* SIGSEGV */
            case 19: /* SIGTSTP */
            case 26: /* SIGVTALRM */
            case 32: /* not defined */
            case 33: /* not defined */
                if (verbose) {
                    fprintf(stderr,
                        "%s: handler skipped for signal=%d (%s).\n",
                        pgm_name_str,
                        sig,
                        all_signal_name[sig]);
                }
                break;
            default:
                if (sigaction(sig, &act, NULL) == -1) {
                    fprintf(stderr,
                        "%s: Addition of signal handler for signal=%d (%s) failed: %m\n",
                        pgm_name_str,
                        sig,
                        all_signal_name[sig]);
                    exit(EXIT_FAILURE);
                }
                if (verbose)
                    fprintf(stderr,
                        "%s: Signal handler added for signal=%d (%s).\n",
                        pgm_name_str,
                        sig,
                        all_signal_name[sig]);

                /* Display log message about adding signal handler */

                time(&now);
                localtime_r(&now, &now_tm);
                num_bytes = strftime(
                    current_time_str,
                    DATE_BUFFER_SIZE,
                    "%F %T",
                    &now_tm);
                
                msg_buffer[0] = '\0';
                if (pgm_name_len < MSG_BUFFER_SIZE)
                    strncat(msg_buffer, pgm_name_str, pgm_name_len);
                if (num_bytes > 0 && strlen(msg_buffer) + num_bytes + 1 < MSG_BUFFER_SIZE) {
                    strcat(msg_buffer, " ");
                    strncat(msg_buffer, current_time_str, num_bytes);
                }
                if (strlen(msg_buffer) + strlen(msg_sig_hndlr_added_start) < MSG_BUFFER_SIZE)
                    strcat(msg_buffer, msg_sig_hndlr_added_start);
                if (strlen(msg_buffer) + strlen(all_signal_name[sig]) < MSG_BUFFER_SIZE)
                    strcat(msg_buffer, all_signal_name[sig]);
                if (strlen(msg_buffer) + strlen(msg_sig_hndlr_added_end) < MSG_BUFFER_SIZE)
                    strcat(msg_buffer, msg_sig_hndlr_added_end);
                msg_len = strlen(msg_buffer);
                if (msg_len > 0)
                    write(log_fd, msg_buffer, msg_len);
                break;
        }
    }

    /* Display log message about idling starting */

    time(&now);
    localtime_r(&now, &now_tm);
    num_bytes = strftime(
        current_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &now_tm);
    
    msg_buffer[0] = '\0';
    if (pgm_name_len < MSG_BUFFER_SIZE)
        strncat(msg_buffer, pgm_name_str, pgm_name_len);
    if (num_bytes > 0 && strlen(msg_buffer) + num_bytes + 1 < MSG_BUFFER_SIZE) {
        strcat(msg_buffer, " ");
        strncat(msg_buffer, current_time_str, num_bytes);
    }
    if (strlen(msg_buffer) + strlen(msg_idle_started) < MSG_BUFFER_SIZE)
        strcat(msg_buffer, msg_idle_started);
    msg_len = strlen(msg_buffer);
    if (msg_len > 0)
        write(log_fd, msg_buffer, msg_len);

    if (verbose)
        fprintf(stderr,
            "%s: Started idling for %ld seconds.\n",
            pgm_name_str,
            idle_for);
    for (long i = 0; i < idle_for; i++) {
        sleep(1);
    }

    /* Display log message about idling ending */

    time(&now);
    localtime_r(&now, &now_tm);
    num_bytes = strftime(
        current_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &now_tm);
    
    msg_buffer[0] = '\0';
    if (pgm_name_len < MSG_BUFFER_SIZE)
        strncat(msg_buffer, pgm_name_str, pgm_name_len);
    if (num_bytes > 0 && strlen(msg_buffer) + num_bytes + 1 < MSG_BUFFER_SIZE) {
        strcat(msg_buffer, " ");
        strncat(msg_buffer, current_time_str, num_bytes);
    }
    if (strlen(msg_buffer) + strlen(msg_idle_ended) < MSG_BUFFER_SIZE)
        strcat(msg_buffer, msg_idle_ended);
    msg_len = strlen(msg_buffer);
    if (msg_len > 0)
        write(log_fd, msg_buffer, msg_len);

    if (verbose)
        fprintf(stderr,
            "%s: Idling ended after %ld seconds.\n",
            pgm_name_str,
            idle_for);

    /* Enable interrupts to be handled */

    if (verbose)
        fprintf(stderr,
            "%s: Enable interrupts to be handled.\n",
            pgm_name_str);

    /* Display log message about unblocking of signals */

    time(&now);
    localtime_r(&now, &now_tm);
    num_bytes = strftime(
        current_time_str,
        DATE_BUFFER_SIZE,
        "%F %T",
        &now_tm);
    
    msg_buffer[0] = '\0';
    if (pgm_name_len < MSG_BUFFER_SIZE)
        strncat(msg_buffer, pgm_name_str, pgm_name_len);
    if (num_bytes > 0 && strlen(msg_buffer) + num_bytes + 1 < MSG_BUFFER_SIZE) {
        strcat(msg_buffer, " ");
        strncat(msg_buffer, current_time_str, num_bytes);
    }
    if (strlen(msg_buffer) + strlen(msg_sig_unblocked) < MSG_BUFFER_SIZE)
        strcat(msg_buffer, msg_sig_unblocked);
    msg_len = strlen(msg_buffer);
    if (msg_len > 0)
        write(log_fd, msg_buffer, msg_len);

    /* Unblock signals */

    if (sigprocmask(SIG_UNBLOCK, &block_set, &prev_mask) == -1) {
        fprintf(stderr,
            "%s: Unable to remove mask for signals: %m\n",
            pgm_name_str);
        exit(EXIT_FAILURE);
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