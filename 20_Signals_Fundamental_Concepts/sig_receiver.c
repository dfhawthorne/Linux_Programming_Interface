/* --------------------------------------------------------------------------
 * Exercise 20-1:
 * Replace the use of signal() by sigaction() in the program in Listing 20-7.
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
 * Static global variables for use by call-back routines.
 * -------------------------------------------------------------------------- */

static int signal_count[NSIG];
static volatile sig_atomic_t sigint_received = 0;
static char *pgm_name       = NULL;
static int verbose          = 0;


/* --------------------------------------------------------------------------
 * This program is only terminated through the SIGTERM signal.
 * -------------------------------------------------------------------------- */

static void terminate_pgm(int signal) {
    if (verbose)
        fprintf(stderr, "%s: Program terminated normally\n", pgm_name);

    if (fflush(stdout) == -1){
        fprintf(stderr, "%s: unable to flush STDOUT: %m\n", pgm_name);
    }

    exit(EXIT_SUCCESS);
}

/* --------------------------------------------------------------------------
 * Signal handler
 * -------------------------------------------------------------------------- */

static void signal_handler(int signal) {
    switch (signal) {
        case SIGINT:
            sigint_received = 1;
            break;
        default:
            signal_count[signal]++;
            break;
    }
    if (verbose)
        fprintf(stderr,
            "%s: signal_handler caught signal=%d\n",
            pgm_name,
            signal);
}   /* signal_handler */

/* --------------------------------------------------------------------------
 * Main routine
 * -------------------------------------------------------------------------- */

int main(int argc, char* argv[]) {
    int   sleep_time     =  0;
    char *full_pgm_name  = NULL;
    char *pgm_name       = NULL;
    int   ignore_signals[NSIG];

    /* Static variables used to parse passed arguments */

    static int show_help        = 0;
    static int verbose          = 0;
    static int sleep_time_found = 0;
    static int use_signal       = 0;
    static int ignore_signal    = 0;

    static struct option long_options[] = {
        {"help",          no_argument,       &show_help,        1 },
        {"verbose",       no_argument,       &verbose,          1 },
        {"sleep-time",    required_argument, &sleep_time_found, 1 },
        {"use-signal",    no_argument,       &use_signal,       1 },
        {"use-sigaction", no_argument,       &use_signal,       0 },
        {"ignore",        required_argument, &ignore_signal,    1 },
        {0,               0,                 0,                 0 }
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
    for (int i = 0; i < NSIG; i++) ignore_signals[i] = 0;
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
            case 2: /* --sleep-time=<s> */
                if (optarg == NULL) {
                    fprintf(stderr,
                    "%s: No argument supplied for --sleep-time\n",
                    pgm_name);
                    exit(EXIT_FAILURE);
                }
                sleep_time = atoi(optarg);
                break;
            case 3: /* --use-signal */
            case 4: /* --use-sigaction */
                break;
            case 5: /* --ignore=<i> */
                if (optarg == NULL) {
                    fprintf(stderr,
                    "%s: No argument supplied for --ignore\n",
                    pgm_name);
                    exit(EXIT_FAILURE);
                }
                int sig = atoi(optarg);
                if (sig > 0 && sig < NSIG)
                    ignore_signals[sig] = 1;
                else {
                    fprintf(stderr,
                    "%s: Invalid signal (%d) supplied for --ignore\n",
                    pgm_name,
                    sig);
                    exit(EXIT_FAILURE);
                }
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
\tCounts signals received until a SIGINT is received.\n\
\t--help\t\tShows this help message.\n\
\t--ignore\tIgnore this signal. Can be specified multiple times.\n\
\t--sleep-time s\tSleep time to block all signals\n\t\t\tOptional.\n\
\t--use-signal\tUse signal to establish signal handlers (usually SIGINT).\n\
\t--use-sigaction\tUse sigaction to establish signal handlers.\n\
\t\t\tDefault. This option is exclusive of --use-signal.\n\
\t--verbose\tShows verbose output.\n",
            pgm_name);
        exit(EXIT_SUCCESS);
    }

    /* ----------------------------------------------------------------------
     * Validate supplied parameters.
     * The initial values of the various variables are used to indicate if
     * the parameter was passed or not.
     * ---------------------------------------------------------------------- */

    if (verbose) {
        if (sleep_time_found)
            fprintf(stderr,
                "%s: sleep_time=%d\n",
                pgm_name,
                sleep_time);
        else
            fprintf(stderr,
                "%s: No sleep time was specified.\n",
                pgm_name);
        if (ignore_signal) {
            for (int sig = 1; sig < NSIG; sig++)
                fprintf(stderr,
                    "%s: signal (%d) will %sbe ignored.\n",
                    pgm_name,
                    sig,
                    ignore_signals[sig] ? "" : "not ");
        }
    }

    /* ----------------------------------------------------------------------
     * Initialise environment
     * ---------------------------------------------------------------------- */

    pid_t receiver_pid = getpid();
    if (verbose) fprintf(stderr, "%s: PID=%ld\n", pgm_name, (long)receiver_pid);
    printf("%s: PID=%ld\n", pgm_name, (long)receiver_pid);

    if (ignore_signal) {
        for (int sig = 1; sig < NSIG; sig++) {
            if (!ignore_signals[sig]) continue;
            printf("%s: signal (%d) will be ignored.\n",
                pgm_name,
                sig);
        }
    } else {
        printf("%s: No signals will be ignored.\n", pgm_name);
    }
    
    /* Set signal handler for all signals */

    if (verbose)
        fprintf(stderr, "%s: Set signal handler for all signals\n", pgm_name);
    
    static struct sigaction new_signal_action;

    new_signal_action.sa_flags   = 0;
    sigemptyset(&new_signal_action.sa_mask);

    if (verbose)
        fprintf(stderr,
            "%s: using %s to establish signal handlers.\n",
            pgm_name,
            (use_signal) ? "signal" : "sigaction");

    for (int sig = 1; sig < NSIG; sig++) {
        if (ignore_signals[sig]) {
            if (verbose)
                fprintf(stderr, "%s: ignoring signal (%d)\n", pgm_name, sig);
            if (use_signal)
                if (signal(sig, SIG_IGN) == SIG_ERR) {
                    fprintf(stderr,
                        "%s: ignoring signal=%d through signal() failed: %m\n",
                        pgm_name,
                        sig);
                } else {
                    if (verbose) {
                        fprintf(stderr,
                            "%s: ignoring signal=%d through signal().\n",
                            pgm_name,
                            sig);
                    }
                }
            else {
                new_signal_action.sa_handler = SIG_IGN;
                if (sigaction(sig, &new_signal_action, NULL) == -1) {
                    fprintf(stderr,
                        "%s: ignoring signal=%d through sigaction() failed: %m",
                        pgm_name,
                        sig);
                } else {
                    if (verbose)
                        fprintf(stderr,
                            "%s: ignoring signal=%d through sigaction().\n",
                            pgm_name,
                            sig);
                }
            }
            continue;
        }
        switch (sig) {
            case  9: /* SIGKILL */
            case 19: /* SIGTSTP */
            case 32:
            case 33:
                if (verbose) {
                    fprintf(stderr,
                        "%s: handler skipped for signal=%d using %s.\n",
                        pgm_name,
                        sig,
                        (use_signal) ? "signal" : "sigaction");
                }
                break;
            case 15: /* SIGTERM */
                if (use_signal)
                    if (signal(sig, terminate_pgm) == SIG_ERR) {
                        fprintf(stderr,
                            "%s: adding of signal handler (terminate_pgm) for sig=%d through signal() failed: %m\n",
                            pgm_name,
                            sig);
                    } else {
                        if (verbose) {
                            fprintf(stderr,
                                "%s: signal handler (terminate_pgm) added for signal=%d through signal().\n",
                                pgm_name,
                                sig);
                        }
                    }
                else {
                    new_signal_action.sa_handler = terminate_pgm;
                    if (sigaction(sig, &new_signal_action, NULL) == -1) {
                        fprintf(stderr,
                            "%s: adding of signal handler (terminate_pgm) for signal=%d through sigaction() failed: %m",
                            pgm_name,
                            sig);
                    } else {
                        if (verbose)
                            fprintf(stderr,
                                "%s: signal handler (terminate_pgm) added for signal=%d through sigaction()\n",
                                pgm_name,
                                sig);
                    }
                }
                break;
            default:
                if (use_signal) {
                    if (signal(sig, signal_handler) == SIG_ERR) {
                        fprintf(stderr,
                            "%s: adding of signal handler (signal_handler) for sig=%d through signal() failed: %m\n",
                            pgm_name,
                            sig);
                    } else {
                        if (verbose)
                            fprintf(stderr,
                                "%s: signal handler (signal_handler) added for signal=%d through signal().\n",
                                pgm_name,
                                sig);
                    }
                } else {
                    new_signal_action.sa_handler = signal_handler;
                    if (sigaction(sig, &new_signal_action, NULL) == -1) {
                        fprintf(stderr,
                            "%s: adding of signal handler (signal_handler) for signal=%d through sigaction() failed: %m",
                            pgm_name,
                            sig);
                    } else {
                        if (verbose)
                            fprintf(stderr,
                                "%s: signal handler (signal_handler) added for signal=%d through sigaction()\n",
                                pgm_name,
                                sig);
                    }
                }
                break;
        }
    }

    if (verbose)
        fprintf(stderr, "%s: Signal handler has been set for all signals\n", pgm_name);

    /* ----------------------------------------------------------------------
     * If a sleep time was specified, temporarily block all signals, sleep
     * (while another process sends us signals), and then display the mask of
     * pending systems and unblock all signals.
     * ---------------------------------------------------------------------- */

    if (sleep_time_found) {
        if (verbose) fprintf(stderr, "%s: sleep required\n", pgm_name);
        sigset_t all_signals;
        if (sigfillset(&all_signals) == -1) {
            fprintf(stderr,
                "%s: Failed to fill signal set: %m\n",
                pgm_name);
            exit(EXIT_FAILURE);
        }
        if (sigprocmask(SIG_SETMASK, &all_signals, NULL) == -1) {
            fprintf(stderr,
                "%s: Failed to set blocking signal mask: %m\n",
                pgm_name);
            exit(EXIT_FAILURE);
        }
        if (verbose) {
            fprintf(stderr, "%s: blocked all signals while sleeping.\n", pgm_name);
            fprintf(stderr, "%s: sleeping for %d seconds.\n", pgm_name, sleep_time);
        }
        printf("%s: sleeping for %d seconds.\n", pgm_name, sleep_time);
        sleep(sleep_time);

        static sigset_t pending_mask;
        if (sigpending(&pending_mask) == -1) {
            fprintf(stderr, "%s: Unable to retrieve pending signals: %m\n", pgm_name);
            exit(EXIT_FAILURE);
        }

        if (verbose) fprintf(stderr, "%s: pending signals are:\n", pgm_name);
        printf("%s: pending signals are:\n", pgm_name);
        int num_pending_signals = 0;
        for (int sig = 1; sig < NSIG; sig++) {
            if (sigismember(&pending_mask, sig)) {
                num_pending_signals++;
                if (verbose)
                    fprintf(stderr,
                        "%s: signal (%d) found in pending mask. %d found so far.\n",
                        pgm_name,
                        sig,
                        num_pending_signals);
                printf("\t\t%3d (%s)\n", sig, strsignal(sig));
            }
        }
        if (num_pending_signals == 0) {
            if (verbose) fprintf(stderr, "%s: No pending signals found.\n", pgm_name);
            printf("\t\t<empty pending signal set>\n");
        }

        /* unblock all signals */

        if (verbose) fprintf(stderr, "%s: unblock all signals.\n", pgm_name);
        sigset_t empty_signal_mask;
        if (sigemptyset(&empty_signal_mask) == -1) {
            fprintf(stderr, "%s: Unable to create empty signal set: %m\n", pgm_name);
            exit(EXIT_FAILURE);
        }
        if (sigprocmask(SIG_SETMASK, &empty_signal_mask, NULL) == -1) {
            fprintf(stderr, "%s: Unable to remove all signal masks: %m\n", pgm_name);
            exit(EXIT_FAILURE);
        }
        if (verbose) fprintf(stderr, "%s: sleep processing completed.\n", pgm_name);
    }

    /* Loop until SIGINT caught */

    if (verbose) fprintf(stderr, "%s: loop until SIGINT caught.\n", pgm_name);
    while (1) if (sigint_received) break;
    if (verbose) fprintf(stderr, "%s: SIGINT caught.\n", pgm_name);

    /* ----------------------------------------------------------------------
     * Display number of signals received.
     * ---------------------------------------------------------------------- */

    int no_signals_caught = 1;
    for (int sig = 1; sig < NSIG; sig++) {
        if (verbose)
            fprintf(stderr,
                "%s: sig (%3d) caught %6d time(s)\n",
                pgm_name,
                sig,
                signal_count[sig]);
        if (signal_count[sig] != 0) {
            no_signals_caught = 0;
            printf("%s: signal %3d caught %6d time(s)\n",
                pgm_name,
                sig,
                signal_count[sig]);
        }
    }
    if (no_signals_caught)
        printf("%s: No signals were caught.\n", pgm_name);

    if (verbose) fprintf(stderr, "%s: Completed.\n", pgm_name);

    exit(EXIT_SUCCESS);
}