/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2026.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* thread_incr.c

   This program employs two POSIX threads that increment the same global
   variable, without using any synchronization method. As a consequence,
   updates are sometimes lost.

   See also thread_incr_mutex.c.
*/
#include <pthread.h>
#include "tlpi_hdr.h"

typedef struct {
    int uid;
    int loop_limit;
} arg_t;

static volatile int glob = 0;   /* "volatile" prevents compiler optimizations
                                   of arithmetic operations on 'glob' */
static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    arg_t *arg_ptr = (arg_t *) arg;
    int loc, j;

    for (j = 0; j < arg_ptr -> loop_limit; j++) {
        loc = glob;
        loc++;
        glob = loc;
        printf("Thread %d: glob=%d\n", arg_ptr -> uid, glob);
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    arg_t parm_1, parm_2;
    int s;

    parm_1.loop_limit = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;

    parm_1.uid = 1;
    s = pthread_create(&t1, NULL, threadFunc, &parm_1);
    if (s != 0)
        errExitEN(s, "pthread_create");
    parm_2.loop_limit = parm_1.loop_limit;
    parm_2.uid = 2;
    s = pthread_create(&t2, NULL, threadFunc, &parm_2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
