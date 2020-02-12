/******************************************************************************\
* Exercise 12-3:                                                               *
* Write a program that lists all processes that have a particular file         *
* pathname open.                                                               *
*                                                                              *
* Parameters:                                                                  *
*     path name to search for                                                  *
\******************************************************************************/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************\
* Main routine                                                                 *
\******************************************************************************/

int
main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "No path name entered\n");
        exit(1);
    }
    
    char *resolved_path = realpath(argv[1], NULL);
    if (resolved_path == NULL)
    {
        fprintf(stderr, "Supplied path (%s) cannot be resolved: %m\n", argv[1]);
        exit(1);
    }

    #ifdef DEBUG
    fprintf(stderr, "Resolved path for '%s' is '%s'\n", argv[1], resolved_path);
    #endif
    
    free(resolved_path);
    exit(0);
}
