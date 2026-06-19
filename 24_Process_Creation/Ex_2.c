/* -------------------------------------------------------------------------- *
 * Exercise 24-2:                                                             *
 *                                                                            *
 * Write a program to demonstrate that a vfork(), the child process can       *
 * close a file descriptor (e.g. descriptor 0) without affecting the          * 
 * corresponding file descriptor in the parent.                               *
 * -------------------------------------------------------------------------- */
 
#define _GNU_SOURCE
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define PARENT_SEM_NAME "/Ex_24_2_parent_sem"

// Global variables shared by processes

unsigned int verbose = 0;   // Level of verbosity of messages for tracing

int main(int argc, char *argv[]) {
    unsigned int linenum = 0;       // Used for error messages to pinpoint failing code
    sem_t *parent_sem    = NULL;    // Semaphore for child to signal parent
    
    // The command argument, -v, enables verbose tracing
    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    // Check semaphore for child to signal parent

    if (verbose) fprintf(stderr, "Parent: Create semaphore for child to signal parent\n");

    sem_unlink(PARENT_SEM_NAME);    // Clean up any previous failed runs
    linenum = __LINE__ + 1;
    parent_sem = sem_open(PARENT_SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (parent_sem == NULL) {
        error_at_line(
            EXIT_FAILURE, errno, __FILE__, linenum,
            "Call to sem_open() failed for parent"
            );
    }

    // Check status of FD 0 before FORK

    if (verbose) fprintf(stderr, "Parent: Check status of FD 0 before vfork()\n");

    struct stat statbuf;
    linenum = __LINE__ + 1;
    if (fstat(0, &statbuf) == -1) {
        int errno_save = errno;

        sem_unlink(PARENT_SEM_NAME);
        error_at_line(
            EXIT_FAILURE, errno_save, __FILE__, linenum,
            "Call to fstat() failed for parent"
            );
    }
    if (verbose) fprintf(stderr, "Parent: Status of FD 0 before vfork() successful\n");

    /* ---------------------------------------------------------------------- *
     * Fork process into parent and child                                     *
     * ---------------------------------------------------------------------- */

    if (verbose) fprintf(stderr, "Parent: Before vfork()\n");
    linenum = __LINE__ + 1;
    pid_t pid = vfork();    // Save PID as a process-local variable
    if (pid < 0) {
        int errno_save = errno;

        sem_unlink(PARENT_SEM_NAME);
        error_at_line(
            EXIT_FAILURE, errno_save, __FILE__, linenum,
            "Call to vfork() failed for parent"
            );
    }

    /* ---------------------------------------------------------------------- *
     * After FORK                                                             *
     * ---------------------------------------------------------------------- */

    if (pid == 0) { // Child process
        if (verbose) fprintf(stderr, "Child: Close FD 0\n");
        close(0);
        if (verbose) fprintf(stderr, "Child: Signaling parent\n");
        linenum = __LINE__ + 1;
        if (sem_post(parent_sem) == -1) {
            error_at_line(
                EXIT_FAILURE, errno, __FILE__, linenum,
                "Post to parent's semaphore failed for child"
            );
        }
        if (verbose) fprintf(stderr, "Child: Exiting\n");
        exit(EXIT_SUCCESS);
    } else {        // Parent process
        if (verbose) fprintf(stderr, "Parent: Waiting for child to signal\n");
        if (sem_wait(parent_sem) == -1) {
            int errno_save = errno;

            sem_unlink(PARENT_SEM_NAME);
            error_at_line(
                EXIT_FAILURE, errno_save, __FILE__, linenum,
                "Wait for semaphore failed for parent"
            );
        }
        if (verbose) fprintf(stderr, "Parent: Check status of FD 0 after child closes FD 0\n");
        linenum = __LINE__ + 1;
        if (fstat(0, &statbuf) == -1) {
            int errno_save = errno;

            sem_unlink(PARENT_SEM_NAME);
            error_at_line(
                EXIT_FAILURE, errno_save, __FILE__, linenum,
                "Call to fstat() failed for parent"
                );
        }
        if (verbose) fprintf(stderr, "Parent: Status of FD 0 after vfork() successful\n");
    }

    // -------------------------------------------------------------------------
    // Clean up is done by parent
    // -------------------------------------------------------------------------

    wait(NULL);
    sem_unlink(PARENT_SEM_NAME);
    return 0;
 
    
    exit(EXIT_SUCCESS);
}
