/* -------------------------------------------------------------------------- *
 * Exercise 15-7: Implement chattr(1) command except for -R, -V, and -v       *
 *                options.                                                    *
 *                                                                            *
 * `man 1 chattr` says:                                                       *
 * SYNOPSIS                                                                   *
 *        chattr [ -RVf ] [ -v version ] [ -p project ] [ mode ] files...     *
 *                                                                            *
 * DESCRIPTION                                                                *
 *        chattr changes the file attributes on a Linux file system.          *
 *                                                                            *
 *        The format of a symbolic mode is +-=[aAcCdDeFijPsStTu].             *
 *                                                                            *
 *        The  operator  '+'  causes the selected attributes to be added to   *
 *        the existing attributes of the files; '-' causes them to  be  re‐   *
 *        moved;  and  '='  causes  them to be the only attributes that the   *
 *        files have.                                                         *
 *                                                                            *
 *        The letters 'aAcCdDeFijPsStTu' select the new attributes for  the   *
 *        files:  append only (a), no atime updates (A), compressed (c), no   *
 *        copy on write (C), no dump  (d),  synchronous  directory  updates   *
 *        (D),  extent  format (e), case-insensitive directory lookups (F),   *
 *        immutable (i), data journalling (j), project hierarchy  (P),  se‐   *
 *        cure  deletion (s), synchronous updates (S), no tail-merging (t),   *
 *        top of directory hierarchy (T), and undeletable (u).                *
 *                                                                            *
 *        The following attributes are read-only,  and  may  be  listed  by   *
 *        lsattr(1)  but not modified by chattr: encrypted (E), indexed di‐   *
 *        rectory (I), inline data (N), and verity (V).                       *
 *                                                                            *
 *        Not all flags are supported or utilized by all filesystems; refer   *
 *        to  filesystem-specific  man pages such as btrfs(5), ext4(5), and   *
 *        xfs(5) for more filesystem-specific details.                        *
 *                                                                            *
 * OPTIONS                                                                    *
 *        -R     Recursively change attributes  of  directories  and  their   *
 *               contents.                                                    *
 *                                                                            * 
 *        -V     Be verbose with chattr's output and print the program ver‐   *
 *               sion.                                                        *
 *                                                                            *
 *        -f     Suppress most error messages.                                *
 *                                                                            *
 *        -v version                                                          *
 *               Set the file's version/generation number.                    *
 *                                                                            *
 *        -p project                                                          *
 *               Set the file's project number.                               *
 * -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int opt;
    int args_done = 0;
    int verbosity = 0;
    int suppress_errors = 0;
    int file_version = 0;
    int project_num = 0;
    char *mode = NULL;
    
    while (((opt = getopt(argc, argv, ":RVfv:p:")) != -1) && !args_done)
    {
        switch (opt)
        {
            case 'R':
                fprintf(stderr,
                    "Usage: %s %s\n",
                       argv[0],
                       "[ -RVf ] [ -v version ] [ -p project ] [ mode ] files..."
                       );
                fprintf(stderr, "-R is not implemented\n");
                exit(EXIT_FAILURE);
                break;
            case 'V':
                verbosity++;
                break;
            case 'f':
                suppress_errors = 1;
                break;
            case 'v':
                file_version = atoi(optarg);
                break;
            case 'p':
                project_num = atoi(optarg);
                break;
            default: /* '?' */
                args_done = 1;
                break;
       }
    }
    
    if (optind >= argc)
    {
        fprintf(stderr, "Expecting file names\n");
        exit(EXIT_FAILURE);
    }
    
    switch (*argv[optind])
    {
        case '+':
        case '-':
        case '=':
            mode = argv[optind];
            break;
        default:
            mode = NULL;
            break;
    }

    if (verbosity)
    {
        fprintf(stderr, "Version: 1.0\n");
        fprintf(stderr,
            "Errors: %s\n",
            (suppress_errors) ? "suppressed" : "displayed"
            );
        fprintf(stderr, "File version: ");
        if (file_version)
        {
            fprintf(stderr, "%d\n", file_version);
        }
        else
        {
            fprintf(stderr, "not changed\n");
        }
        fprintf(stderr, "Project number: ");
        if (project_num)
        {
            fprintf(stderr, "%d\n", project_num);
        }
        else
        {
            fprintf(stderr, "not changed\n");
        }
        fprintf(stderr, "Mode: %s\n", (mode) ? mode : "Not used");
    }


    exit(EXIT_SUCCESS);
}

