// -----------------------------------------------------------------------------
// Exercise 29-1: (amended version)
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

#include <error.h>
#include <pthread.h>
#include <stdlib.h>

int main() {
    int line_num = 0;
    int err_rtn = 0;
    pthread_t tid;

    tid = pthread_self();   // always succeeds

    line_num = __LINE__ + 1;
    if ((tid != pthread_self()) && (err_rtn = pthread_join(tid, NULL)))
        error_at_line(
            EXIT_FAILURE,
            err_rtn,
            __FILE__,
            line_num,
            "pthread_join() failed"
        );

    return 0;
}