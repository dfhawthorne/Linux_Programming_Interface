// -----------------------------------------------------------------------------
// Exercise 28-1:
//
// Write a program to see how fast the fork() and vfork() system calls are on
// your system. Each child process should immediately exit, and the parent
// should wait() on each child before creating the next. Compare the relative
// differences for these two system calls with those of Table 28-3. The shell
// built-in command time can be used to measure the execution time of a program.
// -----------------------------------------------------------------------------

#define _GNU_SOURCE
#define NUM_ITERATIONS 1000000

#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid;
    pid_t waited_pid;
    int line_num = 0;
    int wstatus = 0;
    int idx = 0;

    for (idx = 0; idx < NUM_ITERATIONS; idx++) {
        line_num = __LINE__ + 1;
        child_pid = vfork();
        switch (child_pid) {
            case -1:
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "fork() failed"
                );
                break;

            case 0:     // child
                _exit(EXIT_SUCCESS);
            
            default:    // parent
                line_num = __LINE__ + 1;
                waited_pid = waitpid(child_pid, &wstatus, 0);
                switch (waited_pid) {
                    case -1:
                        error_at_line(
                            EXIT_FAILURE,
                            errno,
                            __FILE__,
                            line_num,
                            "waitpid() failed"
                        );
                        break;
                    
                    default:
                        break;
                }
        }
    }
}
