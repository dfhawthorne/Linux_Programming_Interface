/* ------------------------------------------------------------------------- *
 * Exercise 13-4                                                             *
 * ------------------------------------------------------------------------- */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("If I had more time, \n");
    write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);
    exit(EXIT_SUCCESS);
}
