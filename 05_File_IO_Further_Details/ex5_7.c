/******************************************************************************\
* Exercise 5-7: Implement readv and writev.                                    *
\******************************************************************************/

#include <errno.h>
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
    ssize_t total_bytes_read = 0,
        space_required = 0;
    unsigned char *buffer = NULL,
        *section = NULL;
    int save_errno;
    
    for (int i = 0; i < iovcnt; i++)
    {
        space_required += iov[i].iov_len;
    }
    
    buffer = malloc(space_required);
    if (buffer == NULL)
    {
        return -1;
    }
    
    total_bytes_read = read(fd, buffer, space_required);
    save_errno       = errno;

    section = buffer;
    for (int i = 0; i < iovcnt; i++)
    {
        ssize_t leng = iov[i].iov_len;
        memcpy(iov[i].iov_base, section, leng);
        section += leng;
    }

    free(buffer);
    errno            = save_errno;
    
    return total_bytes_read;       
}

/******************************************************************************\
* My implementation of writev                                                  *
\******************************************************************************/

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t total_bytes_written = 0,
        space_required = 0;
    unsigned char *buffer = NULL,
        *section = NULL;
    int save_errno;
    
    for (int i = 0; i < iovcnt; i++)
    {
        space_required += iov[i].iov_len;
    }
    
    buffer = malloc(space_required);
    if (buffer == NULL)
    {
        return -1;
    }

    section = buffer;
    for (int i = 0; i < iovcnt; i++)
    {
        ssize_t leng = iov[i].iov_len;
        memcpy(section, iov[i].iov_base, leng);
        section += leng;
    }
    
    total_bytes_written = write(fd, buffer, space_required);
    save_errno          = errno;
    free(buffer);
    errno               = save_errno;
    
    return total_bytes_written;       
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
        
    printf("Data: inode=%ld, x=%d, str[0]='%c', str[99]='%c'\n", (long int)myStruct.st_ino, x, str[0], str[99]);

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
        
    printf("Data: inode=%ld, x=%d, str[0]='%c', str[99]='%c'\n", (long int)myStruct.st_ino, x, str[0], str[99]);

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
