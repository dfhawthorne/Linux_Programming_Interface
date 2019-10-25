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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

extern int optind, opterr, optopt;
extern char *optarg;

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    int opt;
    int append = 0;
    
    while ((opt = getopt(argc, argv, ":a")) != -1) {
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
    
    if (optind >= argc) {
        fprintf(stderr, "Missing file name\n");
        exit(1);
    }
    
    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }
    
    int file_opts = O_WRONLY | O_CREAT;
    
    file_opts |= (append) ? O_APPEND : O_TRUNC;
    
    int fd = open(argv[optind], file_opts, S_IRUSR | S_IWUSR);
    
    if (fd == -1) {
        perror("Unable to open file");
        exit(1);
    }
    
    ssize_t bytes_read = 0;
    
    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytes_written = write(STDOUT_FILENO, buffer, BUFFER_SIZE);
        if (bytes_written == -1) {
            perror("I/O error on write to STDOUT");
            close(fd);
            free(buffer);
            exit(1);
        }
        bytes_written = write(fd, buffer, BUFFER_SIZE);
        if (bytes_written == -1) {
            perror("I/O error on write to TEE");
            close(fd);
            free(buffer);
            exit(1);
        }
    }
    
    if (bytes_read == -1) {
        perror("I/O error on read from STDIN");
    }
    
    close(fd);
    free(buffer);
    
    exit(0);
}