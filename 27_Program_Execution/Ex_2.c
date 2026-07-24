// -----------------------------------------------------------------------------
// Exercise 27-2:
//
// -----------------------------------------------------------------------------

#define _GNU_SOURCE
#define MAX_INT_STR_SZ 10

#include <errno.h>
#include <error.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int verbose = 0;

void list_cleanup(char **list) {
    char **list_ptr = list;

    while (*list_ptr) {
        free(*list_ptr++);
    }

    free(list);
}

int my_execlp(const char *pathname, ...) {
    int errno_save;
    va_list ap;
    char **argv = NULL;
    char **envp = NULL;
    int num_passed_args = 0;

    if (verbose) fprintf(stderr, "my_execlp started for \"%s\"\n", pathname);

    va_start(ap, pathname);
    for (char *arg_ptr = va_arg(ap, char *); arg_ptr; arg_ptr = va_arg(ap, char *)) {
        num_passed_args++;
    }
    va_end(ap);

    if (verbose) fprintf(stderr, "my_execlp: num passed args=%d\n", num_passed_args);

    argv = (char **)malloc((num_passed_args+1)*sizeof(char *));
    if (argv == NULL) return -1;

    for (int arg_idx = 0; arg_idx <= num_passed_args; arg_idx++) // include trailing NULL ptr
        argv[arg_idx] = (char *)NULL;    // initialise list

    va_start(ap, pathname);
    for (int arg_idx = 0; arg_idx < num_passed_args; arg_idx++) {
        char *passed_arg = va_arg(ap, char *);
        if (verbose) fprintf(stderr, "my_execlp: arg #%d = \"%s\"\n", arg_idx, passed_arg);
        argv[arg_idx] = (char *)malloc(strlen(passed_arg)+1);
        if (argv[arg_idx] == NULL) {
            errno_save = errno;
            list_cleanup(argv);
            errno = errno_save;
            return -1;
        }
        strcpy(argv[arg_idx], passed_arg);
    }
    va_end(ap);

    return execve(pathname, argv, envp);
}

int main(int argc, char *argv[]) {
    int line_num = 0;

    if ((argc > 1) && (!strcmp(argv[1],"-v"))) verbose = 1;

    line_num = __LINE__ + 1;
    pid_t child_pid = fork();
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
        case 0:     // child process
            if (verbose) fprintf(stderr, "Child started\n");
            line_num = __LINE__ + 1;
            int rc = my_execlp("./Ex_2_child", "./Ex_2_child", "string", "2000", "c", (char *)NULL);
            if (rc == -1) {
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "my_execlp() failed"
                );
                _exit(EXIT_FAILURE);
            }
            break;
        default:    // parent process
            printf("Parent spawned PID=%ld\n", (long)child_pid);
            break;
    }
    return 0;
}
