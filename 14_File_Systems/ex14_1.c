/* --------------------------------------------------------------------------
 | Exercise 14-1: Measures the time required to create and then remove a
 |   large number of 1-byte files from a single directory. The program should
 |   create files of the form xNNNNNN, where NNNNNN is replaced by a random
 |   six-digit number.
 |
 |   The files can be created in random (default) or increasing order.
 |
 |   The files are always deleted in increasing order.
 * -------------------------------------------------------------------------- */

#include <error.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/statfs.h>
#include <sys/times.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* --------------------------------------------------------------------------
 | Displays usage with an optional message.
 * -------------------------------------------------------------------------- */

static void usage_error(char *pgm_name, char *msg)
{
    if (msg != NULL) fprintf(stderr, "%s\n", msg);
  
    fprintf(stderr, "%s: [-n <num_files>] [-s] [-v] <dir_name>\n", pgm_name);
    fprintf(stderr, "-n: Number of files to create - defaults to 10\n");
    fprintf(stderr, "-s: create files in increasing order\n");
    fprintf(stderr, "-v: Displays debugging information\n");
    
    exit(EXIT_FAILURE);
}

/* --------------------------------------------------------------------------
 | Creates a one-byte file with chosen name.
 * -------------------------------------------------------------------------- */

static void create_file(char *dir_name, long file_num, int verbosity)
{
    char new_file_name[1024];
    
    sprintf(new_file_name, "%s/X%06ld", dir_name, file_num);
    
    if (verbosity > 0)
    {
        fprintf(stderr, "Creating %s\n", new_file_name);
    }
    
    int fd = open(new_file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        fprintf(stderr, "Creation of %s failed: %m\n", new_file_name);
    }
    else
    {
        char *buffer = " ";
        if (write(fd, buffer, 1) == -1)
        {
            fprintf(stderr, "Writing to %s failed: %m\n", new_file_name);
        }
        close(fd);
    }
}

/* --------------------------------------------------------------------------
 * Main program
 * -------------------------------------------------------------------------- */
 
int main(int argc, char *argv[])
{

    /* Parse command arguments                                                */
    
    int opt;
    int verbosity          = 0;
    int random_creation    = 1;
    int num_files_required = 10;
    int show_help          = 0;
    
    while ((opt = getopt(argc, argv, "hn:sv")) != -1) {
        switch (opt) {
            case 'h':
                show_help          = 1;
                break;
            case 's':
                random_creation    = 0;
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
           random_creation,
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
    char *output_dir_name = argv[optind];

    if (verbosity > 0)
    {
        fprintf(stderr,
            "output directory name argument = %s\n",
            output_dir_name);
    }
    
    /* Get details about file system where the directory is located */
    
    struct statfs fs_info;
    
    if (statfs(output_dir_name, &fs_info) == -1)
    {
        fprintf(stderr, "Failed to get file system info: %m\n");
        exit(EXIT_FAILURE);
    }
    
    if (verbosity > 0)
    {
        fprintf(stderr, "FS Type         : 0x%08lX\n", (long)fs_info.f_type);
        fprintf(stderr, "FS free i-nodes : %ld\n", (long)fs_info.f_ffree);
    }

    /* ----------------------------------------------------------------------
     | Start collecting timing information
     * ---------------------------------------------------------------------- */
     
    long clockTicks = sysconf(_SC_CLK_TCK);  /* Clocks per tick for conversion */
    struct tms start_t;
    clock_t start_clockTime = clock();

    if (clockTicks == -1)
    {
        fprintf(stderr, "sysconf(_SC_CLK_TCK) failed: %m\n");
        exit(EXIT_FAILURE);
    }
    if (start_clockTime  == -1)
    {
        fprintf(stderr, "clock() failed: %m\n");
        exit(EXIT_FAILURE);
    }
    if (times(&start_t)  == -1)
    {
        fprintf(stderr, "times() failed: %m\n");
        exit(EXIT_FAILURE);
    }

    /* ----------------------------------------------------------------------
     | Create the requested number of files
     * ---------------------------------------------------------------------- */
     
    if (random_creation) {
        srandom(start_clockTime);
    }
     
    for (int i = 0; i < num_files_required; i++)
    {
        long file_num = random_creation ? random() % 1000000 : i;
        create_file(output_dir_name, file_num, verbosity);
    }
    
    /*------------------------------------------------------------------------*\
    | Print statistics for later analysis                                      |
    |  1. File system type                                                     |
    |  2. Create files in random order?                                        |
    |  3. Number of files created                                              |
    |  4. Wall clock time in seconds                                           |
    |  5. User time in seconds                                                 |
    |  6. System time in seconds                                               |
    \*------------------------------------------------------------------------*/
    
    struct tms end_t;
    clock_t end_clockTime = clock();
    
    if (end_clockTime  == -1)
    {
        fprintf(stderr, "clock() failed: %m\n");
        exit(EXIT_FAILURE);
    }
    if (times(&end_t)  == -1)
    {
        fprintf(stderr, "times() failed: %m\n");
        exit(EXIT_FAILURE);
    }
    printf("0x%08lX\t%c\t%ld\t%.6f\t%.6f\t%.6f\n",
        (long)fs_info.f_type,
        (random_creation) ? 'r' : 's',
        num_files_required,
        (double) (end_clockTime   - start_clockTime)   / CLOCKS_PER_SEC,
        (double) (end_t.tms_utime - start_t.tms_utime) / clockTicks,
        (double) (end_t.tms_stime - start_t.tms_stime) / clockTicks
        );

    /* ----------------------------------------------------------------------
     | Delete all files in the directory
     * ---------------------------------------------------------------------- */

    char cmd[1024];
    sprintf(cmd, "find %s -type f -delete", output_dir_name);
    if (verbosity > 0)
    {
        fprintf(stderr, "Clean up cmd: %s\n", cmd);
    }
    system(cmd);
    
    exit(EXIT_SUCCESS);
}