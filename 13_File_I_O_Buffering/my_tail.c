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
    int verbosity          = 0;
    int num_lines_required = 10;
    int show_help          = 0;
    
    while ((opt = getopt(argc, argv, "hn:v")) != -1) {
        switch (opt) {
            case 'h':
                show_help          = 1;
                break;
            case 'n':
                num_lines_required = atoi(optarg);
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
           "num_lines=%d; verbosity=%d; argc=%d; optind=%d\n",
           num_lines_required,
           verbosity,
           argc,
           optind);
    }
    
    if (num_lines_required < 1)
    {
        usage_error(argv[0], "Number of lines requested should be >= 1");
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

    /* Open and read input file                                               */
    
    int fd = open(file_name, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr,
            "opening file %s: %m\n",
            file_name);
        exit(EXIT_FAILURE);
    }
    
    /* Get physical characteristics of input file */
    
    struct stat stats_buffer;
    
    if (fstat(fd, &stats_buffer) == -1)
    {
        fprintf(stderr,
            "failed to get stats for file %s: %m\n",
            file_name);
        close(fd);
        exit(EXIT_FAILURE);
    }

    long block_size      = (long)stats_buffer.st_blksize;
    long file_size       = (long)stats_buffer.st_size;
    long last_blk_offset = (file_size/block_size)*block_size;
    int  num_blocks      = file_size/block_size + 1;
    if (verbosity > 0)
    {
        fprintf(stderr,
                "Block size=%ld, file size=%ld, offset to last block=%ld, #blocks=%d\n",
                block_size,
                file_size,
                last_blk_offset,
                num_blocks);
    }

    /* Maintain a stack of I/O buffers as we read backwards through the input file */
    
    char **buffers       = (char **)malloc(num_blocks*sizeof(char *));
    if (buffers == NULL)
    {
        fprintf(stderr,
            "failed to allocate array of I/O buffer pointers: %m\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_blocks; i++)
    {
        buffers[i] = NULL;
    }

    /* Read backwards through the input file to find the requested number of lines */
    
    int  num_buffers_loaded = 0;
    long blk_offset         = last_blk_offset;
    int  num_lines_found    = 0;
    char *cp                = NULL;
     
    while (num_buffers_loaded < num_blocks)
    {
        buffers[num_buffers_loaded] = (char *)malloc(block_size+1); /* Allow for NULL at end */
        if (buffers[num_buffers_loaded] == NULL)
        {
            fprintf(stderr,
                "failed to allocate I/O buffer: %m\n");
            close(fd);
            for (int i = 0; i < num_blocks; i++)
            {
                if (buffers[i] != NULL)
                {
                    free(buffers[i]);
                }
            }
            free(buffers);
            exit(EXIT_FAILURE);
        }
    
        if (verbosity > 0) fprintf(stderr, "Seeking offset #%ld\n", blk_offset);
        if (lseek(fd, (off_t)blk_offset, SEEK_SET) == (off_t)-1) {
            fprintf(stderr,
                "failed to lseek to offset %ld: %m\n",
                blk_offset);
            close(fd);
            for (int i = 0; i < num_blocks; i++)
            {
                if (buffers[i] != NULL)
                {
                    free(buffers[i]);
                }
            }
            free(buffers);
            exit(EXIT_FAILURE);
        }
    
        if (verbosity > 0) fprintf(stderr, "Reading block into buffer #%d at %p\n", num_buffers_loaded, buffers[num_buffers_loaded]);
        ssize_t bytes_read      = read(fd, (void *)(buffers[num_buffers_loaded]), (size_t)block_size);
   
        if (bytes_read == -1)
        {
            fprintf(stderr,
                "failed to read from file: %m\n");
            close(fd);
            for (int i = 0; i < num_blocks; i++)
            {
                if (buffers[i] != NULL)
                {
                    free(buffers[i]);
                }
            }
            free(buffers);
            exit(EXIT_FAILURE);
        }

        buffers[num_buffers_loaded][bytes_read]   = '\0'; /* Put in teminating null */

        for (cp  = buffers[num_buffers_loaded]  + bytes_read - 1;
             cp >= buffers[num_buffers_loaded];
             cp--)
        {
            if (*cp == '\n') num_lines_found++;
            if (num_lines_found == num_lines_required + 1)
            {
                cp++;
                num_lines_found--;
                break;
            }
        }

        if (verbosity > 0) fprintf(stderr, "num_lines_found=%d\n", num_lines_found);

        num_buffers_loaded++;
    
        if (num_lines_found == num_lines_required) break;
        
        blk_offset -= block_size;
        if (blk_offset < 0) break;
    }
    
    if (cp != NULL)
    {
        if (verbosity > 0) fprintf(stderr, "print partial buffer at %p\n", cp);
        printf(cp);
        for (int i = num_buffers_loaded - 2; i >= 0; i--)
        {
            if (verbosity > 0) fprintf(stderr, "print buffer #%d at %p\n", i, buffers[i]);
            printf(buffers[i]);
        }
    }
        
    close(fd);
    for (int i = 0; i < num_blocks; i++)
    {
        if (buffers[i] != NULL)
        {
            free(buffers[i]);
        }
    }
    free(buffers);
     
    exit(EXIT_SUCCESS);
}
