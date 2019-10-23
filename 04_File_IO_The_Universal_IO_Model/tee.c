/******************************************************************************\
*                                                                              *
* NAME                                                                         *
*        tee  -  read  from  standard  input and write to standard output and  *
*        files                                                                 *
*                                                                              *
* SYNOPSIS                                                                     *
*        tee [OPTION]... [FILE]...                                             *
*                                                                              *
* DESCRIPTION                                                                  *
*        Copy standard input to each FILE, and also to standard output.        *
*                                                                              *
*        -a, --append                                                          *
*               append to the given FILEs, do not overwrite                    *
*                                                                              *
*        -i, --ignore-interrupts                                               *
*               ignore interrupt signals                                       *
*                                                                              *
*        -p     diagnose errors writing to non pipes                           *
*                                                                              *
*        --output-error[=MODE]                                                 *
*               set behavior on write error.  See MODE below                   *
*                                                                              *
*        --help display this help and exit                                     *
*                                                                              *
*        --version                                                             *
*               output version information and exit                            *
*                                                                              *
*    MODE determines behavior with write errors on the outputs:                *
*        'warn' diagnose errors writing to any output                          *
*                                                                              *
*        'warn-nopipe'                                                         *
*               diagnose errors writing to any output not a pipe               *
*                                                                              *
*        'exit' exit on error writing to any output                            *
*                                                                              *
*        'exit-nopipe'                                                         *
*               exit on error writing to any output not a pipe                 *
*                                                                              *
*        The default MODE for the -p option is  'warn-nopipe'.   The  default  *
*        operation  when  --output-error is not specified, is to exit immedi‐ *
*        ately on error writing to a pipe, and diagnose errors writing to non  *
*        pipe outputs.                                                         *
*                                                                              *
\******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int optind, opterr, optopt;
extern char *optarg;

int main(int argc, char *argv[])
{
    int opt;
    int append = 0;
    
    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
            case 'a':
                append = 1;
                break;
            case '?':
                fprintf(stderr, "Unknown option (%c)\n", optopt);
                exit(1);
            default:
                fprintf(stderr, "Unexpected case in parsing of options\n");
                exit(1);
        }
    }
    exit(0);
}