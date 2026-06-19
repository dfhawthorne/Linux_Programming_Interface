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
#define CHILD_SEM_NAME  "/Ex_24_2_child_sem"

// Global variables shared by processes

unsigned int verbose = 0;   // Level of verbosity of messages for tracing

int main(int argc, char *argv[]) {
    unsigned int linenum = 0;   // Used for error messages to pinpoint failing code
    
    // The command argument, -v, enables verbose tracing
    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    // Check status of FD 0 before FORK

    if (verbose) fprintf(stderr, "Parent: Check status of FD 0 before vfork()\n");

    struct stat statbuf;
    linenum = __LINE__ + 1;
    if (fstat(0, &statbuf) == -1) {
        error_at_line(
            EXIT_FAILURE, errno, __FILE__, linenum,
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
        error_at_line(
            EXIT_FAILURE, errno, __FILE__, linenum,
            "Call to vfork() failed for parent"
            );
    }

    /* ---------------------------------------------------------------------- *
     * After FORK                                                             *
     * ---------------------------------------------------------------------- */

    if (pid == 0) { // Child process
        if (verbose) fprintf(stderr, "Child: Close FD 0\n");
        close(0);
        if (verbose) fprintf(stderr, "Child: Exiting\n");
        exit(EXIT_SUCCESS);
    } else {        // Parent process
        if (verbose) fprintf(stderr, "Parent: After vfork()\n");
        if (verbose) fprintf(stderr, "Parent: Check status of FD 0 after child closes FD 0\n");
        linenum = __LINE__ + 1;
        if (fstat(0, &statbuf) == -1) {
            error_at_line(
                EXIT_FAILURE, errno, __FILE__, linenum,
                "Call to fstat() failed for parent"
                );
        }
        if (verbose) fprintf(stderr, "Parent: Status of FD 0 after vfork() successful\n");
    }

    // -------------------------------------------------------------------------
    // Clean up is done by parent
    // -------------------------------------------------------------------------

    wait(NULL);
    return 0;
 
    
    exit(EXIT_SUCCESS);
}
