/******************************************************************************\
* Exercise 5-7: Implement readv and writev.                                    *
\******************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define STR_SIZE 100

/******************************************************************************\
* My implementation of readv                                                   *
\******************************************************************************/

ssize_t my_readv(int fd, const struct iovec *iov, int iovcnt)
{
    return readv(fd, iov, iovcnt);
}

/******************************************************************************\
* My implementation of writev                                                  *
\******************************************************************************/

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt)
{
    return writev(fd, iov, iovcnt);
}

/******************************************************************************\
* Test harnass based on t_readv.c                                              *
* See http://man7.org/tlpi/code/online/dist/fileio/t_readv.c.html              *
\******************************************************************************/

int t_writev(const char *file_name)
{
    int fd;
    struct iovec iov[3];
    struct stat myStruct;       /* First buffer */
    int x;                      /* Second buffer */
    char str[STR_SIZE];         /* Third buffer */
    ssize_t numWritten, totRequired;

    fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open %s: %m\n", file_name);
        return 1;
    }

    totRequired = 0;

    if (fstat(fd, &myStruct) == -1)
    {
        fprintf(stderr, "Stat of %s failed: %m\n", file_name);
    } else {
        memset(&myStruct, 0, sizeof(struct stat));
    }
    iov[0].iov_base = &myStruct;
    iov[0].iov_len = sizeof(struct stat);
    totRequired += iov[0].iov_len;

    x = STR_SIZE;
    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    totRequired += iov[1].iov_len;

    memset(str, 'X', STR_SIZE);
    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    totRequired += iov[2].iov_len;

    numWritten = my_writev(fd, iov, 3);
    if (numWritten == -1)
    {
        fprintf(stderr, "Writev failed: %m\n");
        return 1;
    }

    if (numWritten < totRequired)
        printf("Wrote fewer bytes than requested\n");

    printf("total bytes requested: %ld; bytes written: %ld\n",
            (long) totRequired, (long) numWritten);
    return 0;
}

/******************************************************************************\
* Test harnass based on t_readv.c                                              *
* See http://man7.org/tlpi/code/online/dist/fileio/t_readv.c.html              *
\******************************************************************************/

int t_readv(const char *file_name)
{
    int fd;
    struct iovec iov[3];
    struct stat myStruct;       /* First buffer */
    int x;                      /* Second buffer */
    char str[STR_SIZE];         /* Third buffer */
    ssize_t numRead, totRequired;

    fd = open(file_name, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open %s: %m\n", file_name);
        return 1;
    }

    totRequired = 0;

    iov[0].iov_base = &myStruct;
    iov[0].iov_len = sizeof(struct stat);
    totRequired += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    totRequired += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    totRequired += iov[2].iov_len;

    numRead = my_readv(fd, iov, 3);
    if (numRead == -1)
    {
        fprintf(stderr, "Readv failed: %m\n");
        return 1;
    }

    if (numRead < totRequired)
        printf("Read fewer bytes than requested\n");

    printf("total bytes requested: %ld; bytes read: %ld\n",
            (long) totRequired, (long) numRead);
    return 0;
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char *argv[])
{
    const char *file_name = "test.dat";
    
    if (t_writev(file_name) == 0)
    {
        printf("Test write succeeded\n");
        if (t_readv(file_name) == 0)
        {
            printf("Test read succeeded\n");
        } else {
            printf("Test read failed\n");
        }
    } else {
        printf("Test write failed\n");
    }
    
    exit(0);
}
