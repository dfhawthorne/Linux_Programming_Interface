/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/*************************************************************************\
* Exercise 6-2.                                                           *
\*************************************************************************/

/* longjmp.c

   Demonstrate the use of setjmp() and longjmp() to perform a nonlocal goto.

   Usage: longjmp [x]

   The presence or absence of a command-line argument determines which of two
   functions (f1() or f2()) we will longjmp() from.
*/
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static jmp_buf env;

static void
f2(void)
{
    longjmp(env, 2);
}

static void
f1(int argc)
{
    if (argc == 1)
        longjmp(env, 1);
    f2();
}

void test_jmp(int argc)
{
    switch (setjmp(env)) {
    case 0:     /* This is the return after the initial setjmp() */
        printf("Calling f1() after initial setjmp()\n");
        break;

    case 1:
        printf("We jumped back from f1()\n");
        break;

    case 2:
        printf("We jumped back from f2()\n");
        break;
    }
    printf("test_jmp(%d) finished\n", argc);
}

int
main(int argc, char *argv[])
{
    test_jmp(argc);
    f1(argc);
    exit(0);
}
