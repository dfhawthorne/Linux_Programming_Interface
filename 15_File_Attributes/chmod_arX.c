/* -------------------------------------------------------------------------- *
 | Exercise 15-6: Write a program to perform the equivalent of chmod a+rX.    |
 |                                                                            |
 | man 1 chmod says:                                                          |
 | "...execute/search only if the file  is  a directory or already has        |
 | execute permission for some user (X)..."                                   |
 * -------------------------------------------------------------------------- */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int
main(int argc, char *argv[])
{
    struct stat sb;
    int fname = 1;                /* Location of filename argument in argv[] */

    if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0))
    {
        fprintf(stderr, "%s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[fname], &sb) == -1)
    {
        fprintf(stderr, "stat: %m\n");
        exit(EXIT_FAILURE);
    }
    
    mode_t x_mode = S_IXUSR | S_IXGRP | S_IXOTH;
    mode_t r_mode = S_IRUSR | S_IRGRP | S_IROTH;
    mode_t mode = sb.st_mode & 0777;
    
    if (S_ISREG(sb.st_mode))
    {
        mode |= r_mode;
        if (sb.st_mode & x_mode != 0)
        {
            mode |= x_mode;
        }
        if (chmod(argv[fname], mode) == -1)
        {
            fprintf(stderr, "chmod: %m\n");
            exit(EXIT_FAILURE);
        }
    }
    
    if (S_ISDIR(sb.st_mode))
    {
        mode |= x_mode | r_mode;
        if (chmod(argv[fname], mode) == -1)
        {
            fprintf(stderr, "chmod: %m\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}

