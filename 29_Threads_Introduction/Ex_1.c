// -----------------------------------------------------------------------------
// Exercise 29-1:
//
// What possible outcomes might there be if a thread executes the following
// code:
//
//    pthread_join(pthread_self(), NULL)
//
// Write a program to see what actually happens on Linux. If we have a variable,
// tid, containing a thread ID, how can a thread prevent itself from making a
// call, pthread_join(tid, NULL), that is equivalent to the above statement.
// -----------------------------------------------------------------------------

#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <stdlib.h>

int main() {
    int line_num = 0;

    line_num = __LINE__ + 1;
    if (pthread_join(pthread_self(), NULL))
        error_at_line(
            EXIT_FAILURE,
            errno,
            __FILE__,
            line_num,
            "pthread_join() failed"
        );

    return 0;
}