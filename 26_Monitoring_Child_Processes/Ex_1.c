/* ************************************************************************** *
 * Exercise 26-1:                                                             *
 *                                                                            *
 * Write a program to verify that when a child's parent terminates, a call to *
 * `getppid()` returns 1 (the process ID of `init`).                          *
 *                                                                            *
 * ************************************************************************** */
 
#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Auxiliary variables for error and tracing management
    int verbose = 0;
    int line_num = 0;
    int errno_save = 0;
    // Program specific variables
    pid_t parent_pid = 0;
    int pipe_fd[2];
    char pipe_buf[1];
    ssize_t pipe_buf_read;

    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    if (verbose) fprintf(stderr, "Parent: Create parent <-> child pipe\n");

    line_num = __LINE__ + 1;
    if (pipe(pipe_fd) == -1) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, line_num, "pipe() failed");
    }

    if (verbose) fprintf(stderr, "Parent: About to fork\n");
    
    line_num = __LINE__ + 1;
    pid_t pid = fork();
    errno_save = errno;
    
    switch (pid) {
        case -1:
            error_at_line(EXIT_FAILURE, errno_save, __FILE__, line_num, "fork() failed");
            break;
        
        case 0: // Child process
            if (verbose) fprintf(stderr, "Child: close write end of pipe.\n");
            close(pipe_fd[1]); // ignore errors for this pipe
            
            if (verbose) fprintf(stderr, "Child: waiting for parent to close read end of pipe.\n");
            line_num = __LINE__ + 1;
            pipe_buf_read = read(pipe_fd[0], pipe_buf, sizeof(pipe_buf));

            switch (pipe_buf_read) {
                case -1:
                    error_at_line(EXIT_FAILURE, errno, __FILE__, line_num, "read() failed");
                    break;
                
                case 0:
                    if (verbose) fprintf(stderr, "Child: Parent has closed read end of pipe\n");
                    break;
                
                default:
                    error_at_line(EXIT_FAILURE, 0, __FILE__, line_num, "read() returned unexpected data");
                    break;
            }

            close(pipe_fd[0]); // Ignore errors from close of read end of pipe
            
            if (verbose) fprintf(stderr, "Child: Get process id of parent\n");
            line_num = __LINE__ + 1;
            parent_pid = getppid();
            if (parent_pid == -1) {
                error_at_line(EXIT_FAILURE, errno, __FILE__, line_num, "getppid() failed");
            }

            printf("Process ID of parent is now %d\n", parent_pid);

            exit(EXIT_SUCCESS);
            break;

        default:  // parent process
            if (verbose) fprintf(stderr, "Parent: about to exit\n");
            _exit(EXIT_SUCCESS);  // exit() cleans up children
            break;
    }
    
    if (verbose) fprintf(stderr, "Parent: end of program\n");

}