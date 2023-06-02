/* --------------------------------------------------------------------------
 * Based on Listing 20-6. Sending multiple signals.
 * -------------------------------------------------------------------------- */

#define _GNU_SOURCE
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/* --------------------------------------------------------------------------
 * Main routine
 * -------------------------------------------------------------------------- */

int main(int argc, char* argv[]) {
    long num_signals     =  1;
    int sig_number       = -1;
    int last_sig_number  = -1;
    pid_t receiver_pid   =  0;
    char *full_pgm_name  = NULL;
    char *pgm_name       = NULL;

    /* Static variables used to parse passed arguments */

    static int show_help             = 0;
    static int verbose               = 0;
    static int last_sig_number_found = 0;
    static int sig_number_found      = 0;
    static int receiver_pid_found    = 0;

    static struct option long_options[] = {
        {"help",        no_argument,       &show_help,             1 },
        {"verbose",     no_argument,       &verbose,               1 },
        {"last-signal", required_argument, &last_sig_number_found, 1 },
        {"num-signals", required_argument, 0,                      0 },
        {"pid",         required_argument, &receiver_pid_found,    1 },
        {"signal",      required_argument, &sig_number_found,      1 },
        {0,             0,                 0,                      0 }
    };

    full_pgm_name  = strdup(argv[0]);
    if (full_pgm_name == NULL) {
        fprintf(stderr,
            "%s: unable to duplicate program name: %m\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }
    pgm_name       = basename(full_pgm_name);
    if (pgm_name == NULL) {
        fprintf(stderr,
            "%s: Unable to extract base name for program: %m\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }
    int option_index = 0;
    while (1) {
        int c = getopt_long_only(argc, argv, "", long_options, &option_index);
        if (c == -1) break;
        if (c != 0) {
            fprintf(stderr,
                "%s: Argument parsing failed.\n",
                pgm_name);
            exit(EXIT_FAILURE);
        }
        if (verbose)
            fprintf(stderr,
                "%s: c=%d; option_index=%d; optarg=%s\n",
                pgm_name,
                c,
                option_index,
                optarg);
        switch (option_index) {
            case 0: /* --help */
            case 1: /* --verbose */
                break;
            case 2: /* --last-signal=<l> */
                if (optarg == NULL) {
                    fprintf(stderr,
                    "%s: No argument supplied for --last-signal\n",
                    pgm_name);
                    exit(EXIT_FAILURE);
                }
                last_sig_number = atoi(optarg);
                break;
            case 3: /* --num-signals=<n> # default 1 */
                if (optarg == NULL) {
                    fprintf(stderr,
                    "%s: No argument supplied for --num-signals\n",
                    pgm_name);
                    exit(EXIT_FAILURE);
                }
                num_signals = atol(optarg);
                break;
            case 4: /* --pid=<p> */
                if (optarg == NULL) {
                    fprintf(stderr,
                    "%s: No argument supplied for --pid\n",
                    pgm_name);
                    exit(EXIT_FAILURE);
                }
                receiver_pid = atol(optarg);
                break;
            case 5: /* --signal=<s> */
                if (optarg == NULL) {
                    fprintf(stderr,
                    "%s: No argument supplied for --signal\n",
                    pgm_name);
                    exit(EXIT_FAILURE);
                }
                sig_number = atoi(optarg);
                break;
            default:
                fprintf(stderr, "%s: Invalid arg found %d\n", pgm_name, option_index);
                break;
        }
    }

    /* ----------------------------------------------------------------------
     * Show help message, if required.
     * ---------------------------------------------------------------------- */

    if (show_help) {
        fprintf(stderr,
            "%s [options ...]\n\
\tSends signal a number of times to the process identified by pid, then the \
optional last signal.\n\
\t--help\t\tShows this help message.\n\
\t--last-signal s\tLast signal to send\n\t\t\tRequired.\n\
\t--num-signals n\tNumber of signals to send\n\t\t\tDefaults to 1\n\
\t--pid p\t\tPID of process to receive signals\n\t\t\tRequired.\n\
\t--signal s\tSignal number to send\n\t\t\tOptional.\n\
\t--verbose\tShows verbose output.\n",
            pgm_name);
        exit(EXIT_SUCCESS);
    }

    /* ----------------------------------------------------------------------
     * Validate supplied parameters.
     * The initial values of the various variables are used to indicate if
     * the parameter was passed or not.
     * ---------------------------------------------------------------------- */

    if (! sig_number_found) {
        fprintf(stderr,
            "%s: No signal number was supplied.\n",
            pgm_name);
        exit(EXIT_FAILURE);
    }
    if (! receiver_pid_found) {
        fprintf(stderr,
            "%s: No receiver PID was supplied.\n",
            pgm_name);
        exit(EXIT_FAILURE);
    }
    if (sig_number < 1 || sig_number >= NSIG) {
        fprintf(stderr,
            "%s: Invalid signal (%d) specified.\n",
            pgm_name,
            sig_number);
        exit(EXIT_FAILURE);
    }
    if (last_sig_number_found && \
        (last_sig_number < 1 || last_sig_number >= NSIG)) {
            fprintf(stderr,
                "%s: Invalid last signal (%d) specified.\n",
                pgm_name,
                last_sig_number);
            exit(EXIT_FAILURE);
    }

    if (verbose) {
        fprintf(stderr,
            "%s: receiver_pid=%ld; num_signals=%ld\n",
            pgm_name,
            (long)receiver_pid,
            num_signals);
        fprintf(stderr,
            "%s: Main signal (%d) passed is '%s'\n",
            pgm_name,
            sig_number,
            strsignal(sig_number));
        if (last_sig_number_found)
            fprintf(stderr,
                "%s: Last signal (%d) passed is '%s'\n",
                pgm_name,
                last_sig_number,
                strsignal(last_sig_number));
        else
            fprintf(stderr,
                "%s: No last signal was specified.\n",
                pgm_name);
    }
    printf("%s: %ld signals (signal=%d) are to be sent to process (PID=%ld)",
        pgm_name,
        num_signals,
        sig_number,
        (long)receiver_pid);
    if (last_sig_number_found)
        printf(" with a last signal of %d", last_sig_number);
    printf(".\n");

    /* ----------------------------------------------------------------------
     * Send the required number of signals
     * ---------------------------------------------------------------------- */

    for (int i = 0; i < num_signals; i++)
        if (kill(receiver_pid, sig_number) == -1) {
            fprintf(stderr,
                "%s: kill with signal (%d) failed after %d iterations: %m\n",
                pgm_name,
                sig_number,
                i);
            exit(EXIT_FAILURE);
        }

    if (verbose) fprintf(stderr, "%s: Completed.\n", pgm_name);
    printf("%s: Completed.\n", pgm_name);
    
    exit(EXIT_SUCCESS);
}