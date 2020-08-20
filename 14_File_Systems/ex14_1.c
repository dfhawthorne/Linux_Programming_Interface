/* --------------------------------------------------------------------------
 | Exercise 14-1: 
 * -------------------------------------------------------------------------- */

#include <error.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* --------------------------------------------------------------------------
 | Displays usage with an optional message.
 * -------------------------------------------------------------------------- */

static void usage_error(char *pgm_name, char *msg)
{
    if (msg != NULL) fprintf(stderr, "%s\n", msg);
  
    fprintf(stderr, "%s: [-n <num_files>] [-r] [-v] <file_name>\n", pgm_name);
    fprintf(stderr, "-n: Number of files to create - defaults to 10\n");
    fprintf(stderr, "-r: delete files in descending order of creation\n");
    fprintf(stderr, "-v: Displays debugging information\n");
    
    exit(EXIT_FAILURE);
}

/* --------------------------------------------------------------------------
 * Main program
 * -------------------------------------------------------------------------- */
 
int main(int argc, char *argv[])
{

    /* Parse command arguments                                                */
    
    int opt;
    int verbosity          = 0;
    int reverse_deletion   = 0;
    int num_files_required = 10;
    int show_help          = 0;
    
    while ((opt = getopt(argc, argv, "hn:rv")) != -1) {
        switch (opt) {
            case 'h':
                show_help          = 1;
                break;
            case 'r':
                reverse_deletion   = 1;
                break;
            case 'n':
                num_files_required = atoi(optarg);
                break;
            case 'v':
                verbosity++;
                break;
            default: /* '?' */
                usage_error(argv[0], NULL);
        }
    }
    
    if (show_help)
    {
        usage_error(argv[0], NULL);
        exit(EXIT_SUCCESS);
    }

    if (verbosity > 0)
    {
        fprintf(stderr,
           "num_files=%d; reverse=%d; verbosity=%d; argc=%d; optind=%d\n",
           num_files_required,
           reverse_deletion,
           verbosity,
           argc,
           optind);
    }
    
    if (num_files_required < 1)
    {
        usage_error(argv[0], "Number of files requested should be >= 1");
    }

    if (optind >= argc)
    {
        usage_error(argv[0], "Expected one (1) argument after options");
    }
    char *file_name = argv[optind];

    if (verbosity > 0)
    {
        fprintf(stderr,
            "input file name argument = %s\n",
            file_name);
    }    

    exit(EXIT_SUCCESS);
}