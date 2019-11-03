/******************************************************************************\
*                                                                              *
* Exercise 5-6:                                                                *
*                                                                              *
\******************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/******************************************************************************\
* Performs a number of writes before returning                                 *
\******************************************************************************/

int do_writes(const char *file_name, int num_writes)
{
    int fd1 = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd1 == -1)
    {
        fprintf(stderr, "Unable to open %s first time: %m\n", file_name);
        return -1;
    }
    int fd2 = dup(fd1);
    if (fd2 == -1)
    {
        fprintf(stderr, "Unable to duplicate file descriptor (%d): %m\n", fd1);
        return -1;
    }
    int fd3 = open(file_name, O_RDWR);
    if (fd3 == -1)
    {
        fprintf(stderr, "Unable to open %s second time: %m\n", file_name);
        return -1;
    }
    if (num_writes > 0) {
        ssize_t num_written = write(fd1, "Hello,", 6);
        if (num_written < 6)
        {
            fprintf(stderr, "Unable to write %d bytes to fd #%d on write #1: %m\n", 6, fd1);
        }
    }
    if (num_writes > 1) {
        ssize_t num_written = write(fd2, "world", 6);
        if (num_written < 6)
        {
            fprintf(stderr, "Unable to write %d bytes to fd #%d on write #2: %m\n", 6, fd1);
        }
    }
    off_t offset = lseek(fd2, 0, SEEK_SET);
    if (offset == -1)
    {
        fprintf(stderr, "Unable to seek to start of file for fd (%d): %m\n", fd2);
        close(fd1);
        close(fd2);
        close(fd3);
        return -1;
    }
    if (num_writes > 2) {
        ssize_t num_written = write(fd1, "HELLO,", 6);
        if (num_written < 6)
        {
            fprintf(stderr, "Unable to write %d bytes to fd #%d on write #3: %m\n", 6, fd1);
        }
    }
    if (num_writes > 3) {
        ssize_t num_written = write(fd3, "Gidday", 6);
        if (num_written < 6)
        {
            fprintf(stderr, "Unable to write %d bytes to fd #%d on write #4: %m\n", 6, fd3);
        }
    }
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}

/******************************************************************************\
* Prints out contents of test file                                             *
\******************************************************************************/

void print_file(const char *file_name)
{
    int fd = open(file_name, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open %s for printing: %m\n", file_name);
        exit(1);
    }
    char buffer[512];
    ssize_t num_read = read(fd, buffer, 512);
    if (num_read == -1)
    {
        fprintf(stderr, "Unable to read from %s (fd #%d): %m\n", file_name, fd);
        close(fd);
        exit(1);
    }
    close(fd);
    printf("%-*s\n", (int)num_read, buffer);
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char *argv[])
{
    char *file_name = "test.dat";
    
    for (int i = 1; i < 5; i++)
    {
        printf("Now doing %d writes\n", i);
        if (do_writes(file_name, i) == 0)
        {
            printf("The contents, after %d writes, are:\n", i);
            print_file(file_name);
        }
    }
    exit(0);
}
