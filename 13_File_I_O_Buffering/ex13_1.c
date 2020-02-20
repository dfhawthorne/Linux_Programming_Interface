
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    int buffer_size = 1024;
    int opt;
    int verbosity = 0;
    int open_sync = 0;
    
    while ((opt = getopt(argc, argv, "b:sv")) != -1) {
        switch (opt) {
        case 'b':
            buffer_size = atoi(optarg);
            break;
        case 's':
            open_sync = 1;
            break;
        case 'v':
            verbosity++;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-b buffer_size] [-s] [-v] input-file-name output-file-name\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (verbosity > 0)
    {
        printf("buffer_size=%d; verbosity=%d; sync=%d; argc=%d; optind=%d\n",
           buffer_size, verbosity, open_sync, argc, optind);
    }

    if (optind >= argc-1) {
        fprintf(stderr, "Expected two (2) arguments after options\n");
        exit(EXIT_FAILURE);
    }

    if (verbosity > 0)
    {
        printf("input file name argument = %s\n", argv[optind]);
        printf("output file name argument = %s\n", argv[optind+1]);
    }    
    
    
    char *buf = malloc(buffer_size);
    if (buf == NULL)
    {
        fprintf(stderr, "malloc of %d: %m\n", buffer_size);
        exit(EXIT_FAILURE);
    }

    /* Open input and output files */

    inputFd = open(argv[optind], O_RDONLY);
    if (inputFd == -1)
    {
        fprintf(stderr, "opening file %s: %m\n", argv[optind]);
        exit(EXIT_FAILURE);
    }

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    if (open_sync) openFlags |= O_SYNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */
    outputFd = open(argv[optind+1], openFlags, filePerms);
    if (outputFd == -1)
    {
        fprintf(stderr, "opening file %s: %m\n", argv[optind+1]);
        exit(EXIT_FAILURE);
    }

    /* Transfer data until we encounter end of input or an error */

    while ((numRead = read(inputFd, buf, buffer_size)) > 0)
        if (write(outputFd, buf, numRead) != numRead)
        {
            fprintf(stderr, "couldn't write whole buffer: %m\n");
            exit(EXIT_FAILURE);
        }
    if (numRead == -1)
    {
        fprintf(stderr, "read: %m\n");
        exit(EXIT_FAILURE);
    }

    sync();
    
    if (close(inputFd) == -1)
    {
        fprintf(stderr, "close input: %m\n");
        exit(EXIT_FAILURE);
    }
    if (close(outputFd) == -1)
    {
        fprintf(stderr, "close output: %m\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

