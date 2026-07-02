/* -------------------------------------------------------------------------- *
 * Exercise 26-2:                                                             *
 *                                                                            *
 * Suppose that we have three processes related as grandfather, parent, and   *
 * child and that the grandparent doesn't immediatelty perform a `wait()`     *
 * after the parent exits, so that the parent becomes a zombie. When do       *
 * expect the grandchild to adopted by init (so that `getppid()` returns 1):  *
 * after the parent terminates or after the grandparent does a `wait()`?      *
 * Write a program to verify your answer.                                     *
 * -------------------------------------------------------------------------- */

#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int verbose = 0;
    int line_num = 0;
    int errno_save = 0;
    int pipe_fd[2];
    char pipe_buf[1];
    ssize_t pipe_buf_read;
    pid_t grandparent_pid = 0;
    pid_t parent_pid = 0;
    pid_t child_pid = 0;

    // Set level of verbosity
    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    // Grandparent: create pipe for interprocess signalling
    if (verbose) {
        fprintf(
            stderr,
            "Grandparent: create pipe for interprocess signalling.\n"
            );
        }

    line_num = __LINE__ + 1;
    if (pipe(pipe_fd) == -1) {
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "pipe() failed"
            );
        }

    // Grandparent: get process ID
    if (verbose) {
        fprintf(
            stderr,
            "Grandparent: get process ID.\n"
            );
        }

    line_num = __LINE__ + 1;
    if ((grandparent_pid = getpid()) == -1) {
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "getpid() failed"
            );
        }

    printf("Grandparent PID=%d\n", grandparent_pid);

    // Grandparent: create process tree
    if (verbose) {
        fprintf(
            stderr,
            "Grandparent: create process tree.\n"
            );
        }

    line_num = __LINE__ + 1;
    parent_pid = fork();
    errno_save = errno;
    switch (parent_pid) {
        case -1:
            error_at_line(
                EXIT_FAILURE,
                errno_save,
                __FILE__,
                line_num,
                "fork() failed"
                );
            break;

        case 0:  // parent
            line_num = __LINE__ + 1;
            child_pid = fork();
            errno_save = errno;

            switch (child_pid) {
                case -1:
                    error_at_line(
                        EXIT_FAILURE,
                        errno_save,
                        __FILE__,
                        line_num,
                        "fork() failed"
                        );
                    break;

                case 0:  // child

                    if (verbose) {
                        fprintf(
                            stderr,
                            "Child: get parent PID after parent exits.\n"
                            );
                        }

                    line_num = __LINE__ + 1;
                    parent_pid = getppid();

                    if (parent_pid == -1) {
                        error_at_line(
                            EXIT_FAILURE,
                            errno,
                            __FILE__,
                            line_num,
                            "getppid() failed"
                            );
                        }
                    
                    printf("Child: Parent PID=%d\n", parent_pid);

                    if (verbose) fprintf(stderr, "Child: close write end of pipe.\n");
                    close(pipe_fd[1]); // ignore errors for this pipe

                    if (verbose)
                        fprintf(
                            stderr,
                            "Child: waiting for grandparent to close read end of pipe.\n"
                            );

                    line_num = __LINE__ + 1;
                    pipe_buf_read = read(pipe_fd[0], pipe_buf, sizeof(pipe_buf));

                    switch (pipe_buf_read) {
                        case -1:
                            error_at_line(
                                EXIT_FAILURE,
                                errno,
                                __FILE__,
                                line_num,
                                "read() failed"
                                );
                            break;

                        case 0:
                            if (verbose)
                                fprintf(
                                    stderr,
                                    "Child: Grandarent has closed read end of pipe\n"
                                    );
                            break;

                        default:
                            error_at_line(
                                EXIT_FAILURE,
                                0,
                                __FILE__,
                                line_num,
                                "read() returned unexpected data"
                                );
                            break;
                    }

                    close(pipe_fd[0]); // Ignore errors from close of read end of pipe

                    if (verbose) {
                        fprintf(
                            stderr,
                            "Child: get parent PID after grandparent executes wait().\n"
                            );
                        }

                    line_num = __LINE__ + 1;
                    parent_pid = getppid();

                    if (parent_pid == -1) {
                        error_at_line(
                            EXIT_FAILURE,
                            errno,
                            __FILE__,
                            line_num,
                            "getppid() failed"
                            );
                        }

                    printf("Child: Parent PID=%d\n", parent_pid);

                    if (verbose) {
                        fprintf(
                            stderr,
                            "Child: exiting.\n"
                            );
                        }

                    exit(EXIT_SUCCESS);
                    break;
                
                default:  // parent
                    printf("Child PID=%d\n", child_pid);

                    if (verbose) {
                        fprintf(
                            stderr,
                            "Parent: exiting.\n"
                            );
                        }

                    _exit(EXIT_SUCCESS);
                    break;
                }
            break;

        default:  // grandparent
            printf("Parent PID=%d\n", parent_pid);
            break;
        }

    // Exit from grandparent
    if (verbose) {
        fprintf(
            stderr,
            "Grandparent: exiting.\n"
            );
        }

    _exit(EXIT_SUCCESS);
    }