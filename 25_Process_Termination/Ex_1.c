/* --------------------------------------------------------------------------- *
 * Exercise 25-1:                                                              *
 * If a child process makes the call `exit(-1)`, what exit status (as returned *
 * by `WEXITSTATUS()`) will be seen by the parent?                             *
 * --------------------------------------------------------------------------- */

#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int linenum;
    int status;
    
    linenum = __LINE__ + 1;
    pid_t pid = vfork();
    
    switch (pid) {
        case -1:
            error_at_line(EXIT_FAILURE, errno, __FILE__, linenum, "vfork() failed");
            break;
            
        case 0:
            _exit(-1);
            break;
          
        default:
            
            linenum = __LINE__ + 1;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) { // child exited normally
                int code = WEXITSTATUS(status);
                printf("child exit code = %d\n", code);
            } else {
                error_at_line(EXIT_FAILURE, errno, __FILE__, linenum, "child did not exit normally");
            }
            break;
    }
}
