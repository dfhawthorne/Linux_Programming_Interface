/******************************************************************************\
*                                                                              *
* Exercise 5-5:                                                                *
*                                                                              *
\******************************************************************************/


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void desc_fd(int fd)
{
	struct stat statbuf;
	
	int rc = fstat(fd, &statbuf);
	if (rc == -1)
	{
		fprintf(stderr, "Stat for fd #%d failed: %m\n", fd);
		return;
	}
	
	int fd_status = fcntl(fd, F_GETFL);
	if (fd_status == -1)
	{
		fprintf(stderr, "Get file status word for fd #%d failed: %m\n", fd);
		return;
	}
	
	off_t offset = lseek(fd, 0, SEEK_CUR);
	if (offset == -1)
	{
		fprintf(stderr, "Get current location for fd #%d failed: %m\n", fd);
		return;
	}
	
	printf("FD (%d) points to inode #%ld, has a status of 0X%04X, and an offset of %ld\n", fd, statbuf.st_ino, fd_status, (long int)offset);
}

int main(int argc, char *argv[])
{
	int fd1 = open("test.dat", O_RDONLY);
	if (fd1 == -1)
	{
		perror("Failed to open test.dat");
		exit(1);
	}

	printf("Original file descriptor\n");
	desc_fd(fd1);

	int fd2 = dup(fd1);
	if (fd2 == -1)
	{
		perror("Failed to duplicate fd");
	}

	printf("Duplicate file descriptor\n");
	desc_fd(fd2);
	
	off_t offset = lseek(fd2, 0, SEEK_END);
	if (offset == -1)
	{
		fprintf(stderr, "Set current location for fd #%d to EOF failed: %m\n", fd2);
		exit(1);
	}
	
	printf("Both file descriptors after seeking to EOF\n");
	desc_fd(fd1);
	desc_fd(fd2);
	
    exit(0);
}
