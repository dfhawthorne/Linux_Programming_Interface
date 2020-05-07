/* --------------------------------------------------------------------------
 | Exercise 13-5: my simplified version of tail.
 |
 | There is only one (1) optional parameter: -n which defaults to ten (10).
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
  
    fprintf(stderr, "%s: [-n <num_lines>] [-v] <file_name>\n", pgm_name);
    fprintf(stderr, "-n: Number of lines to display - defaults to 10\n");
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
    int verbosity = 0;
    int num_lines = 10;
    
    while ((opt = getopt(argc, argv, "n:v")) != -1) {
        switch (opt) {
        case 'n':
            num_lines = atoi(optarg);
            break;
        case 'v':
            verbosity++;
            break;
        default: /* '?' */
            usage_error(argv[0], NULL);
        }
    }

    if (verbosity > 0)
    {
        printf("num_lines=%d; verbosity=%d; argc=%d; optind=%d\n",
           num_lines, verbosity, argc, optind);
    }

    if (optind >= argc) {
        usage_error(argv[0], "Expected one (1) argument after options");
    }
    char *file_name = argv[optind];

    if (verbosity > 0)
    {
        printf("input file name argument = %s\n", file_name);
    }    

    /* Allocate data structures                                               */

    long *offsets = (long *)malloc(num_lines * sizeof(long));
    if (offsets == NULL) {
        fprintf(stderr, "%s: Unable to allocate offsets array: %m\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int curr_offset_idx = 0;
    int num_lines_read = 0;
    offsets[0] = 0L;
    
    /* Open and read input file                                               */
    
    int fd = open(file_name, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "opening file %s: %m\n", file_name);
        free(offsets);
        exit(EXIT_FAILURE);
    }
    
    struct stat stats_buffer;
    
    if (fstat(fd, &stats_buffer) == -1)
    {
        fprintf(stderr, "failed to get stats for file %s: %m\n", file_name);
        close(fd);
        free(offsets);
        exit(EXIT_FAILURE);
    }

    blksize_t block_size = stats_buffer.st_blksize;
    if (verbosity > 0) fprintf(stderr, "Block size=%ld\n", (long)block_size);
    char *io_buffer = (char *)malloc((long)block_size);
    
    if (io_buffer == NULL)
    {
        fprintf(stderr, "failed to allocate I/O buffer: %m\n");
        close(fd);
        free(offsets);
        exit(EXIT_FAILURE);
    }
    
    close(fd);
    free(offsets);
    free(io_buffer);
    
    exit(EXIT_SUCCESS);
}