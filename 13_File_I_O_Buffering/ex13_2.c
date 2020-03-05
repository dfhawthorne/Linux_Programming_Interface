/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* write_bytes.c

   Write bytes to a file. (A simple program for file I/O benchmarking.)

   Usage: write_bytes file num-bytes buf-size

   Writes 'num-bytes' bytes to 'file', using a buffer size of 'buf-size'
   for each write().

   If compiled with -DUSE_O_SYNC, open the file with the O_SYNC flag,
   so that all data and metadata changes are flushed to the disk.

   If compiled with -DUSE_FDATASYNC, perform an fdatasync() after each write,
   so that data--and possibly metadata--changes are flushed to the disk.

   If compiled with -DUSE_FSYNC, perform an fsync() after each write, so that
   data and metadata are flushed to the disk.
*/

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    size_t buffer_size = 8192L,
           num_bytes   = 8192L,
           thisWrite   = 0L,
           totWritten  = 0L;
    char *buf;
    int fd, openFlags;
    int opt;
    int verbosity = 0;
    int open_sync = 0,
        data_sync = 0,
        file_sync = 0;

    while ((opt = getopt(argc, argv, "b:dfhn:sv")) != -1) {
        switch (opt) {
        case 'b':
            buffer_size = atoi(optarg);
            break;
        case 'd':
            data_sync = 1;
            break;
        case 'f':
            file_sync = 1;
            break;
        case 'h':
            fprintf(stderr, "Usage: %s -h\n", argv[0]);
            fprintf(stderr, "Usage: %s [-b buffer_size] [-d] [-f] [-n num_bytes] [-s] [-v] file-name\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        case 'n':
            num_bytes = atoi(optarg);
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
        printf("buffer_size=%ld; verbosity=%d; data_sync=%c; file_sync=%c; open_sync=%c; num_bytes=%ld; argc=%d; optind=%d\n",
           buffer_size, verbosity, (data_sync?'Y':'N'), (file_sync?'Y':'N'), (open_sync?'Y':'N'), num_bytes, argc, optind);
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected one (1) argument after options\n");
        exit(EXIT_FAILURE);
    }

    if (verbosity > 0)
    {
        printf("output file name argument = %s\n", argv[optind]);
    }    

    buf = malloc(buffer_size);
    if (buf == NULL)
    {
        fprintf(stderr, "malloc: %m\n");
        exit(EXIT_FAILURE);
    }

    openFlags = O_CREAT | O_WRONLY;

    if (open_sync) openFlags |= O_SYNC;

    fd = open(argv[optind], openFlags, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        fprintf(stderr, "open: %m\n");
        exit(EXIT_FAILURE);
    }

    for (totWritten = 0; totWritten < num_bytes;
            totWritten += thisWrite) {
        thisWrite = (buffer_size < num_bytes - totWritten) ? buffer_size : num_bytes - totWritten;

        if (write(fd, buf, thisWrite) != thisWrite)
        {
            fprintf(stderr, "partial/failed write: %m\n");
            exit(EXIT_FAILURE);
        }

        if (file_sync && fsync(fd))
        {
            fprintf(stderr, "fsync: %m\n");
            exit(EXIT_FAILURE);
        }

        if (data_sync && fdatasync(fd))
        {
            fprintf(stderr, "fdatasync: %m\n");
            exit(EXIT_FAILURE);
        }
    }

    if (close(fd) == -1)
    {
        fprintf(stderr, "close: %m");
        exit(EXIT_FAILURE);
    }
    
    /*------------------------------------------------------------------------*\
    | Print statistics for later analysis                                      |
    |  1. Block Size                                                           |
    |  2. Open Synchronisation flag set?                                       |
    |  3. File Synchronisation flag set?                                       |
    |  4. Data Synchronisation flag set?                                       |
    |  5. Number of bytes read                                                 |
    |  6. Wall clock time in seconds                                           |
    |  7. User time in seconds                                                 |
    |  7. System time in seconds                                               |
    \*------------------------------------------------------------------------*/
    
    struct tms t;
    clock_t clockTime = clock();
    long clockTicks = sysconf(_SC_CLK_TCK);  /* Clocks per tick for conversion */
    
    if (clockTicks == -1)
    {
        fprintf(stderr, "sysconf(_SC_CLK_TCK) failed: %m\n");
        exit(EXIT_FAILURE);
    }
    if (clockTime  == -1)
    {
        fprintf(stderr, "clock() failed: %m\n");
        exit(EXIT_FAILURE);
    }
    if (times(&t)  == -1)
    {
        fprintf(stderr, "times() failed: %m\n");
        exit(EXIT_FAILURE);
    }
    printf("%ld\t%c\t%c\t%c\t%ld\t%.2f\t%.2f\t%.2f\n",
        buffer_size,
        (open_sync) ? 's' : '-',
        (file_sync) ? 'f' : '-',
        (data_sync) ? 'd' : '-',
        num_bytes,
        (double) clockTime / CLOCKS_PER_SEC,
        (double) t.tms_utime / clockTicks,
        (double) t.tms_stime / clockTicks
        );
    exit(EXIT_SUCCESS);
}
