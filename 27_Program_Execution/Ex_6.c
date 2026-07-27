// -----------------------------------------------------------------------------
// Exercise 27-6:
//
// Suppose that a parent process has established a handler for SIGCHLD and also
// blocked that signal. Subsequently, one of its children exits, and the parent
// does a wait() to collect the child's status. What happens when the parent
// unblocks SIGCHLD? Write a program to verify your answer. What is the
// revelance for a program calling the system() function?
// -----------------------------------------------------------------------------

#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

// -----------------------------------------------------------------------------
// Signal handler for SIGCHLD
// -----------------------------------------------------------------------------

static void sigchld_handler(int sig, siginfo_t *sig_info, void *ucontext) {
    (void)ucontext;
    printf("sigchld_handler: %d signal caught\n", sig);
    printf("sigchld_handler: child PID=%ld\n", (long)sig_info -> si_pid);
    printf("sigchld_handler: child's real user ID=%ld\n", (long)sig_info -> si_uid);
    if (sig_info -> si_code == CLD_EXITED)
        printf("sigchld_handler: child exited with status=%d\n", sig_info -> si_status);
    else
        printf("sigchld_handler: child did not exit. Signal code=%d\n", sig_info -> si_code);
    printf("sigchld_handler: child user CPU time=%ld\n", (long)sig_info -> si_utime);
    printf("sigchld_handler: child system CPU time=%ld\n", (long)sig_info -> si_stime);
}

// -----------------------------------------------------------------------------
// Main program
// - SIGCHLD is monitored for the purpose of the TLPI exercise
// - SIGUSR1 is used to synchronise parent and child
// -----------------------------------------------------------------------------

int main() {
    int line_num = 0;
    struct sigaction sigchld_action;
    sigset_t process_mask;
    sigset_t sync_mask;
    pid_t child_pid;
    int sig_caught = 0;

    // Set up signal handler for SIGCHLD

    sigchld_action.sa_flags     = SA_SIGINFO;
    sigchld_action.sa_handler   = NULL;
    sigchld_action.sa_sigaction = sigchld_handler;
    sigchld_action.sa_restorer  = NULL;

    line_num = __LINE__ + 1;
    if (sigemptyset(&sigchld_action.sa_mask))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigemptyset() failed for sigchld_action.sa_mask"
        );

    line_num = __LINE__ + 1;
    if (sigaction(SIGCHLD, &sigchld_action, NULL))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigaction() failed for sigchld_handler"
        );
    
    // Block signals (SIGUSR1 and SIGCHLD) for the process

    line_num = __LINE__ + 1;
    if (sigemptyset(&process_mask))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigemptyset() failed for process_mask"
        );

    line_num = __LINE__ + 1;
    if (sigaddset(&process_mask, SIGCHLD))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigaddset() failed for SIGCHLD to process_mask"
        );

    line_num = __LINE__ + 1;
    if (sigaddset(&process_mask, SIGUSR1))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigaddset() failed for SIGUSR1 to process_mask"
        );

    line_num = __LINE__ + 1;
    if (sigprocmask(SIG_BLOCK, &process_mask, NULL))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigprocmask() failed for blocking process_mask"
        );
    
    // Set signal mask (SIGUSR1) for the child to wait on

    line_num = __LINE__ + 1;
    if (sigemptyset(&sync_mask))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigemptyset() failed for sync_mask"
        );

    line_num = __LINE__ + 1;
    if (sigaddset(&sync_mask, SIGUSR1))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "sigaddset() failed for SIGUSR1 to sync_mask"
        );

    line_num = __LINE__ + 1;
    switch (child_pid = fork()) {
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
            line_num = __LINE__ + 1;
            switch (sig_caught = sigwaitinfo(&sync_mask, NULL)) {
                case -1:
                    error_at_line(
                        EXIT_FAILURE,
                        errno,
                        __FILE__,
                        line_num,
                        "sigwaitinfo() failed for child"
                    );
                    break;
                
                default:
                    _exit(EXIT_SUCCESS);
                    break;
            }
            break;
        
        default:    // Parent
            line_num = __LINE__ + 1;
            if (kill(child_pid, SIGUSR1))
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "kill() failed while sending SIGUSR1 to child"
                );
            
            line_num = __LINE__ + 2;
            int wstatus;
            pid_t waited_pid = waitpid(child_pid, &wstatus, 0);
            if (waited_pid == -1)
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "waitpid() failed for child"
                );
            
            printf("Child status=%d\n", wstatus);

            line_num = __LINE__ + 1;
            if (sigprocmask(SIG_UNBLOCK, &process_mask, NULL))
                error_at_line(
                    EXIT_FAILURE,
                    errno,
                    __FILE__,
                    line_num,
                    "sigprocmask() failed for unblocking process_mask"
                );
        
    }

}