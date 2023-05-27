/* --------------------------------------------------------------------------
 * Exercise 19-1:
 * Write a program that logs all file creations, deletions, and renames under
 * the directory named in the command-line argument. The program should
 * monitor events in all of the subdirectories under the specified directory.
 * When a new subdirectory is added under the tree is directory, the set of
 * monitored subirectories should be updated accordingly.
 * -------------------------------------------------------------------------- */

#define _XOPEN_SOURCE 600

#include <ftw.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <time.h>
#include <unistd.h>

/* --------------------------------------------------------------------------
 * Global variables are needed because of call-back routines for signal and 
 * ntfw.
 * -------------------------------------------------------------------------- */

#define MAX_ELEM 100

typedef struct cookie {
    struct cookie *next;
    uint32_t cookie_id;
    char *new_path_name;
    char *old_path_name;
} cookie_t;

static char *pgm_name = NULL;
static char *root_dir_name = NULL;
static int root_dir_fd = -1;
static uint32_t mask = IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_TO | IN_MOVED_FROM;
/* static uint32_t mask = IN_ALL_EVENTS; */
static int verbosity = 0;
static char *elem[MAX_ELEM];
static cookie_t *cookies = NULL;

/* -------------------------------------------------------------------------
 * Exit routine to clean up all dynamically allocated memory
 * ------------------------------------------------------------------------- */

void exit_routine(void) {
    for (int i = 0; i < MAX_ELEM; i++) {
        if (elem[i] != NULL) free(elem[i]);
    }
    for (cookie_t *p = cookies; p == NULL; ) {
        if (p -> old_path_name != NULL) free(p -> old_path_name);
        if (p -> new_path_name != NULL) free(p -> new_path_name);
        cookie_t *next = p -> next;
        free(p);
        p = next;
    }
}

/* ------------------------------------------------------------------------- 
 * Walk the directory tree to add notification events
 * Source modified from nftw_dir_tree.c (pp.360-361) under GPl-v3 licence.
 * ------------------------------------------------------------------------- */

static int                      /* Function called by nftw() */
add_dir_watch(const char *pathname, const struct stat *sbuf, int type,
        struct FTW *ftwb)
{
    int inotify_fd = -1;
    if (type == FTW_NS) {
        fprintf(stderr, "%s: WARNING - Could not stat() file, %s\n", pgm_name, pathname);
    } else {
        if (((sbuf->st_mode & S_IFMT) == S_IFDIR) || ((sbuf->st_mode & S_IFMT) == IN_CREATE)){
            for (int i = 0; i < MAX_ELEM; i++) {
                if (elem[i] == NULL) continue;
                if (strcmp(pathname, elem[i]) == 0) {
                    if (verbosity)
                        fprintf(stderr, "%s: path ('%s') already monitored as %d\n", pgm_name, pathname, i);
                    return 0; /* tell ntfw to continue */
                }
            }
            inotify_fd = inotify_add_watch(root_dir_fd, pathname, mask);
            if (inotify_fd == -1) {
                fprintf(stderr, "%s: Unable to add watch for %s: %m\n", pgm_name, pathname);
            } else {
                if (verbosity) 
                    fprintf(stderr, "%s: added watch for %s with fd=%d\n", pgm_name, pathname, inotify_fd);
                if (inotify_fd >= MAX_ELEM) {
                    fprintf(stderr, "%s: More than maximum elements found\n", pgm_name);
                } else {
                    if (elem[inotify_fd] != NULL) {
                        free(elem[inotify_fd]);
                    }
                    long path_size = strlen(pathname)+1;
                    elem[inotify_fd] = (char *)malloc(path_size);
                    if (elem[inotify_fd] == NULL) {
                        fprintf(stderr, "%s: unable to alocate %ld bytes for %s: %m\n", pgm_name, path_size, pathname);
                    }
                    strncpy(elem[inotify_fd], pathname, path_size);
                }
            }
        } else {
            if (verbosity)
                fprintf(stderr, "%s: skipping %s in NTFW callback\n", pgm_name, pathname);
        }
    }
    return 0;                                   /* Tell nftw() to continue */
}

/* --------------------------------------------------------------------------
 * This program is only terminated through the SIGTERM signal.
 * -------------------------------------------------------------------------- */

static void terminate_pgm(int sig) {
    if ((root_dir_fd != -1) && close(root_dir_fd)) {
        fprintf(stderr, "%s: close of inotify fd failed: %m\n", pgm_name);
    }

    if (verbosity)
        fprintf(stderr, "%s: Program terminated normally\n", pgm_name);

    if (fflush(stdout) == -1){
        fprintf(stderr, "%s: unable to flush STDOUT: %m\n", pgm_name);
    }

    exit(EXIT_SUCCESS);
}

/* ------------------------------------------------------------------------- 
 * Main routine
 * ------------------------------------------------------------------------- */

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define DATE_BUFFER_SIZE 20
#define PATH_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    pgm_name = argv[0];
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t event_batch_size;
    char *p;
    struct inotify_event *event;
    time_t now;
    char current_time_str[DATE_BUFFER_SIZE+1];
    char current_path_name[PATH_BUFFER_SIZE+1];

    /* Establish an exit routine call back to clean up dynamic memory structures */

    if (atexit(exit_routine) != 0) {
        fprintf(stderr, "%s: Unable to activate exit routine: %m\n", pgm_name);
        exit(EXIT_FAILURE);
    } 

    /* ----------------------------------------------------------------------
     * At least a single parameter is expected: name of the root directory
     * tree to be monitored. Multiple root directories are not supported.
     * '-v' is an optional parameter to display verbose messages.
     * ---------------------------------------------------------------------- */

    if (argc < 2) {
        fprintf(stderr, "%s: Expecting a directory name\n", pgm_name);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-v") == 0) {
        verbosity = 1;
        if (argc < 3) {
            fprintf(stderr, "%s: Expecting a directory name\n", pgm_name);
            exit(EXIT_FAILURE);
        }
        root_dir_name = argv[2];
    } else {
        verbosity = 0;
        root_dir_name = argv[1];
    }

    /* This program is terminated by a SIGTERM signal */

    if (signal(SIGTERM, terminate_pgm) == SIG_ERR) {
        fprintf(
            stderr,
            "%s: addition of program terminate signal handler failed: %m\n",
            pgm_name);
        exit(EXIT_FAILURE);
    }

    /* Establish monitoring on all sub-directories in the required directory
     * tree, including the root directory. */

    if ((root_dir_fd = inotify_init()) == -1) {
        fprintf(stderr, "%s: ionitify_init() failed: %m\n", pgm_name);
        exit(EXIT_FAILURE);
    }

    if (verbosity)
        fprintf(stderr, "%s: inotify_init opened FD=%d\n", pgm_name, root_dir_fd);

    if (nftw(root_dir_name, add_dir_watch, 10, 0) == -1) {
        fprintf(stderr, "%s: nftw failed for initial directory: %m\n", pgm_name);
        if (close(root_dir_fd)) {
            fprintf(stderr, "%s: close of inotify fd failed: %m\n", pgm_name);
        }
        exit(EXIT_FAILURE);
    }

    /* ----------------------------------------------------------------------
     * Infinite loop to consume batches of file monitoring events. Exit is 
     * achieved via the SIGTERM signal. See above.
     * The read call is blocking.
     * Return values of 0 and -1 indicate errors.
     * ---------------------------------------------------------------------- */

    for (;;) {
        event_batch_size = read(root_dir_fd, buf, BUF_LEN);
        if (event_batch_size <= 0) {
            fprintf(stderr,
                "%s: read() from inotify (FD=%d) returned %ld!: %m\n",
                pgm_name,
                root_dir_fd,
                (long)event_batch_size);
            exit(EXIT_FAILURE);
        }

        if (verbosity)
            fprintf(stderr,
            "%s: Read %ld bytes from inotify (FD=%d)\n",
            pgm_name,
            (long) event_batch_size,
            root_dir_fd);

        /* Create the current time stamp for prepending to event messages. */

        time(&now);
        size_t num_bytes = strftime(current_time_str, DATE_BUFFER_SIZE, "%F %T", localtime(&now));
        if (num_bytes == 0) fprintf(stderr, "%s: unable to format current time.: %m\n", pgm_name);

        /* Decode next event in the current batch. */

        for (   p = buf;
                p < buf + event_batch_size;
                p += sizeof(struct inotify_event) + event->len ) {
            event = (struct inotify_event *) p;

            /* --------------------------------------------------------------
             * Ensure WD in event falls within the bounds imposed by this
             * program. Also ensure that the WD has a directory name
             * associated with it.
             * -------------------------------------------------------------- */

            if (event -> wd >= MAX_ELEM || event -> wd <= 0) {
                fprintf(stderr,
                    "%s: Invalid WD (%d) found in event - outside of the range 1..%d\n",
                    pgm_name,
                    event -> wd,
                    MAX_ELEM - 1);
                exit(EXIT_FAILURE);
            }

            if (elem[event -> wd] == NULL) {
                fprintf(stderr,
                    "%s: WD (%d) does not have a directory associated with it.\n",
                    pgm_name,
                    event -> wd);
                exit(EXIT_FAILURE);
            }

            /* Construct a path name related to this event. */

            if (event -> len > 0) {
                u_int32_t path_length = strlen(elem[event -> wd]) + strlen(event -> name) + 2;
                if (path_length > PATH_BUFFER_SIZE) {
                    fprintf(stderr,
                        "%s: length of event path length (%ld) exceeds available space (%ld)\n",
                        pgm_name,
                        (long)path_length,
                        (long)PATH_BUFFER_SIZE);
                    exit(EXIT_FAILURE);
                }
                sprintf(current_path_name, "%s/%s", elem[event -> wd], event -> name);
            } else {
                current_path_name[0] = '\0';
            }

            /* --------------------------------------------------------------
             * Debugging information display for event.
             * -------------------------------------------------------------- */

            if (verbosity) {
                fprintf(stderr, "%s: Event found: wd = %3d; ",  pgm_name, event -> wd);
                fprintf(stderr, "cookie = %6d; ", event -> cookie);

                fprintf(stderr, "mask = ");  
                if (event -> mask & IN_ACCESS)        fprintf(stderr, "IN_ACCESS ");
                if (event -> mask & IN_ATTRIB)        fprintf(stderr, "IN_ATTRIB ");
                if (event -> mask & IN_CLOSE_NOWRITE) fprintf(stderr, "IN_CLOSE_NOWRITE ");
                if (event -> mask & IN_CLOSE_WRITE)   fprintf(stderr, "IN_CLOSE_WRITE ");
                if (event -> mask & IN_CREATE)        fprintf(stderr, "IN_CREATE ");
                if (event -> mask & IN_DELETE)        fprintf(stderr, "IN_DELETE ");
                if (event -> mask & IN_DELETE_SELF)   fprintf(stderr, "IN_DELETE_SELF ");
                if (event -> mask & IN_IGNORED)       fprintf(stderr, "IN_IGNORED ");
                if (event -> mask & IN_ISDIR)         fprintf(stderr, "IN_ISDIR ");
                if (event -> mask & IN_MODIFY)        fprintf(stderr, "IN_MODIFY ");
                if (event -> mask & IN_MOVE_SELF)     fprintf(stderr, "IN_MOVE_SELF ");
                if (event -> mask & IN_MOVED_FROM)    fprintf(stderr, "IN_MOVED_FROM ");
                if (event -> mask & IN_MOVED_TO)      fprintf(stderr, "IN_MOVED_TO ");
                if (event -> mask & IN_OPEN)          fprintf(stderr, "IN_OPEN ");
                if (event -> mask & IN_Q_OVERFLOW)    fprintf(stderr, "IN_Q_OVERFLOW ");
                if (event -> mask & IN_UNMOUNT)       fprintf(stderr, "IN_UNMOUNT ");
                fprintf(stderr, "; ");

                fprintf(stderr, "name = '%s'; ", (event -> len > 0) ? event -> name : "(null)");
                fprintf(stderr, "directory = '%s'\n", elem[event -> wd]);
                fprintf(stderr, "%s: current_path_name='%s'\n", pgm_name, current_path_name);
            }

            /* --------------------------------------------------------------
             * Save the cookie state.
             *
             * Assumes events arrive in the order of IN_MOVED_FROM then
             * IN_MOVED_TO
             * An IN_MOVED_TO event without a cookie means that the path was
             * moved in from outside the monitored directories.
             * -------------------------------------------------------------- */

            if (event -> cookie > 0) {
                cookie_t *old_cookie = NULL;
                for (cookie_t *cp = cookies; cp != NULL; cp = cp -> next ) {
                    if ( cp -> cookie_id == event -> cookie ) {
                        old_cookie = cp;
                        break;
                    }
                }

                if ( old_cookie != NULL ) { /* An existing cookie */
                    if (verbosity)
                        fprintf(stderr,
                            "%s: cookie (%ld) already exists.\n",
                            pgm_name,
                            (long)old_cookie -> cookie_id);
                    /* Should be associated with the IN_MOVED_TO event. */
                    if ( ! (event -> mask & IN_MOVED_TO) ) {
                        fprintf(stderr,
                            "%s: Cookie found, but event flag IN_MOVED_TO is not set.\n",
                            pgm_name);
                        exit(EXIT_FAILURE);
                    }
                    u_int32_t path_name_size = strlen(current_path_name) + 1;
                    char *new_path_name = (char *)malloc(path_name_size);
                    if (new_path_name == NULL) {
                        fprintf(stderr,
                        "%s: unable to allocate a cookie path name entry of %d bytes: %m\n",
                        pgm_name,
                        path_name_size);
                        exit(EXIT_FAILURE);
                    }
                    strcpy(new_path_name, current_path_name);
                    old_cookie -> new_path_name = new_path_name;
                    if (verbosity)
                        fprintf(stderr,
                            "%s: Cookie (%d) found with old path='%s' and new path='%s'\n",
                            pgm_name,
                            event -> cookie,
                            old_cookie -> old_path_name,
                            old_cookie -> new_path_name);
                } else { /* No existing cookie */
                    cookie_t *new_cookie = (cookie_t *)malloc( sizeof(cookie_t) );
                    if (new_cookie == NULL) {
                        fprintf(stderr, "%s: unable to allocate a cookie entry: %m\n", pgm_name);
                        exit(EXIT_FAILURE);
                    }
                    new_cookie -> next          = cookies;
                    new_cookie -> cookie_id     = event -> cookie;
                    new_cookie -> old_path_name = NULL;
                    new_cookie -> new_path_name = NULL;
                    cookies                     = new_cookie;
                    long path_size = strlen(current_path_name) + 1;
                    char *path_name = (char *)malloc(path_size);
                    if (path_name == NULL) {
                        fprintf(stderr,
                            "%s: Unable to allocate %ld bytes for path name for cookie (%ld): %m\n",
                            pgm_name,
                            path_size,
                            (long)(event -> cookie));
                        exit(EXIT_FAILURE);
                    }
                    strcpy(path_name, current_path_name);
                    if (event -> mask & IN_MOVED_FROM)
                        new_cookie -> old_path_name = path_name;
                    if (event -> mask & IN_MOVED_TO)
                        new_cookie -> new_path_name = path_name;
                    if (verbosity)
                        fprintf(stderr,
                            "%s: Cookie (%d) created for old path='%s' and new path='%s' with WD=%d\n",
                            pgm_name,
                            event -> cookie,
                            new_cookie -> old_path_name,
                            new_cookie -> new_path_name,
                            event -> wd);
                }   /* no existing cookie */ 
            }   /* Cookie reference in event */

            /* --------------------------------------------------------------
             * Process other events
             * -------------------------------------------------------------- */

            if (event -> mask & IN_DELETE) {
                if (verbosity) fprintf(stderr, "%s: %s deleted\n", pgm_name, current_path_name);
                printf("%s: '%s' deleted\n", current_time_str, current_path_name);
            }   /* IN_DELETE */

            /* IN_DELETE_SELF are duplicates of IN_DELETE for monitored directories */
            
            if (event -> mask & IN_IGNORED) {
                if (verbosity) fprintf(stderr, "%s: Watch on %s removed\n", pgm_name, elem[event -> wd]);
                free(elem[event -> wd]);
                elem[event -> wd] = NULL;
            }   /* IN_IGNORED */

            if (event -> mask & IN_CREATE) {
                if (verbosity) fprintf(stderr, "%s: '%s' created\n", pgm_name, current_path_name);
                printf("%s: '%s' created\n", current_time_str, current_path_name);

                if (event -> mask & IN_ISDIR) {
                    if (nftw(current_path_name, add_dir_watch, 10, 0) == -1) {
                        fprintf(stderr, 
                            "%s: nftw failed for rebuild of directory ('%s'): %m\n",
                            pgm_name,
                            current_path_name);
                        if (close(root_dir_fd)) {
                            fprintf(stderr, "%s: close of inotify fd failed: %m\n", pgm_name);
                        }
                        exit(EXIT_FAILURE);
                    }
                }   /* IN_ISDIR */
            }   /* IN_CREATE */

            /* --------------------------------------------------------------
             * An IN_MOVE_SELF event can refer to a FD that has a saved
             * cookie. This means that an IN_MOVED_FROM event without a
             * corresponding IN_MOVED_TO event. This means that the path was
             * moved outside of a monitored directory.
             * -------------------------------------------------------------- */

            if (event -> mask & IN_MOVE_SELF) {
                if (verbosity)
                    fprintf(stderr,
                        "%s: Updated monitored WD (%d)\n",
                        pgm_name,
                        event -> wd);
            } /* IN_MOVE_SELF */
        } /* Event batch loop */

        /* ---------------------------------------------------------------
        * Clean up cookies for this event batch.
        * ** Possible Bug **
        * If the lifetime of a cookie spans multiple event batches, then
        * this code will remove the data needed snd cause an incorrect
        * monitored event log entries.
        * --------------------------------------------------------------- */

        for (cookie_t *cp = cookies; cp != NULL; ) {
            if (verbosity)
                fprintf(stderr,
                    "%s: Cookie (%ld) found: old path name = '%s'; new path name = '%s'\n",
                    pgm_name,
                    (long)cp -> cookie_id,
                    cp -> old_path_name,
                    cp -> new_path_name);            
            if (cp -> old_path_name != NULL) {
                if (cp -> new_path_name != NULL) {
                    if (verbosity)
                        fprintf(stderr,
                            "%s: '%s' moved to '%s' for cookie (%ld)\n",
                            pgm_name,
                            cp -> old_path_name,
                            cp -> new_path_name,
                            (long)cp -> cookie_id);
                    printf("%s: '%s' moved to '%s'\n",
                        current_time_str,
                        cp -> old_path_name,
                        cp -> new_path_name);
                } else {
                    if (verbosity)
                        fprintf(stderr,
                            "%s: '%s' moved outside of monitored directories for cookie (%ld)\n",
                            pgm_name,
                            cp -> old_path_name,
                            (long)cp -> cookie_id);
                    printf("%s: '%s' moved outside of monitored directories.\n",
                        current_time_str,
                        cp -> old_path_name);
                }
            } else {
                if (cp -> new_path_name != NULL) {
                    if (verbosity)
                        fprintf(stderr,
                            "%s: '%s' moved in from outside of monitored directories for cookie (%ld)\n",
                            pgm_name,
                            cp -> new_path_name,
                            (long)cp -> cookie_id);
                    printf("%s: '%s' moved in from outside the monitored directories\n",
                        current_time_str,
                        cp -> new_path_name);
                    if (nftw(cp -> new_path_name, add_dir_watch, 10, 0) == -1) {
                        fprintf(stderr, 
                            "%s: nftw failed for addition of directory ('%s'): %m\n",
                            pgm_name,
                            cp -> new_path_name);
                        if (close(root_dir_fd)) {
                            fprintf(stderr, "%s: close of inotify fd failed: %m\n", pgm_name);
                        }
                        exit(EXIT_FAILURE);
                    }
                } else {
                    fprintf(stderr,
                        "%s: cookie (%ld) has no path names (either new or old).\n",
                        pgm_name,
                        (long)cp -> cookie_id);
                    exit(EXIT_FAILURE);
                }
            }

            if (cp -> old_path_name != NULL) free(cp -> old_path_name);
            if (cp -> new_path_name != NULL) free(cp -> new_path_name);
            cookie_t *cp_next = cp -> next;
            uint32_t cp_id = cp -> cookie_id;
            free(cp);
            cp = cp_next;
            if (verbosity)
                fprintf(stderr,
                    "%s: Cookie (%ld) removed.\n",
                    pgm_name,
                    (long)cp_id);
        }   /* Clean up cookies */
        cookies = NULL;

    }   /* Infinite loop */

    
} /* main */