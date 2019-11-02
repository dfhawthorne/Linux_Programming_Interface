/******************************************************************************\
*                                                                              *
* Exercise 5-4:                                                                *
*                                                                              *
* NAME                                                                         *
*        dup, dup2 - duplicate a file descriptor                               *
*                                                                              *
* SYNOPSIS                                                                     *
*        int dup(int oldfd);                                                   *
*        int dup2(int oldfd, int newfd);                                       *
*                                                                              *
* DESCRIPTION                                                                  *
*        The dup() system call creates a copy of the file descriptor oldfd,    *
*        using the lowest-numbered unused file descriptor for the new          *
*        descriptor.                                                           *
*                                                                              *
*        After a successful return, the old and new file descriptors may be    *
*        used interchangeably.  They refer to the same open file description   *
*        (see open(2)) and thus share file offset and file status flags; for   *
*        example, if the file offset is modified by using lseek(2) on one of   *
*        the file descriptors, the offset is also changed for the other.       *
*                                                                              *
*        The two file descriptors do not share file descriptor flags (the      *
*        close-on-exec flag).  The close-on-exec flag (FD_CLOEXEC; see         *
*        fcntl(2)) for the duplicate descriptor is off.                        *
*                                                                              *
*    dup2()                                                                    *
*        The dup2() system call performs the same task as dup(), but instead   *
*        of using the lowest-numbered unused file descriptor, it uses the      *
*        file descriptor number specified in newfd.  If the file               *
*        descriptor newfd was previously open, it is silently closed before    *
*        being reused.                                                         *
*                                                                              *
*        The steps of closing and reusing the file descriptor newfd are        *
*        performed atomically.  This is important, because trying to           *
*        implement equivalent functionality using close(2) and dup() would be  *
*        subject to race conditions, whereby newfd might be reused between     *
*        the two steps.  Such reuse could happen because the main program is   *
*        interrupted by a signal handler that allocates a file                 *
*        descriptor, or because a parallel thread allocates a file             *
*        descriptor.                                                           *
*                                                                              *
*        Note the following points:                                            *
*                                                                              *
*                                                                              *
*        *  If oldfd is not a valid file descriptor, then the call fails, and  *
*           newfd is not closed.                                               *
*                                                                              *
*        *  If oldfd is a valid file descriptor, and newfd has the same value  *
*           as oldfd, then dup2() does nothing, and returns newfd.             *
*                                                                              *
* RETURN VALUE                                                                 *
*        On success, these system calls return the new file descriptor.  On    *
*        error, -1 is returned, and errno is set appropriately.                *
*                                                                              *
* ERRORS                                                                       *
*        EBADF  oldfd isn't an open file descriptor.                           *
*                                                                              *
*        EBADF  newfd is out of the allowed range for file descriptors (see    *
*               the discussion of RLIMIT_NOFILE in getrlimit(2)).              *
*                                                                              *
*        EBUSY  (Linux only) This may be returned by dup2() or dup3() during   *
*               a race condition with open(2) and dup().                       *
*                                                                              *
*        EMFILE The per-process limit on the number of open file descriptors   *
*               has been reached (see the discussion of RLIMIT_NOFILE in       *
*               getrlimit(2)).                                                 *
*                                                                              *
* NOTES                                                                        *
*        The error returned by dup2() is different from that returned by       *
*        fcntl(..., F_DUPFD, ...)  when newfd is out of range.  On some        *
*        systems, dup2() also sometimes returns EINVAL like F_DUPFD.           *
*                                                                              *
*        If newfd was open, any errors that would have been reported at        *
*        close(2) time are lost.  If this is of concern, then unless the       *
*        program is single-threaded and does not allocate file descriptors     *
*        in signal handlers the correct approach is not to close newfd         *
*        before calling dup2(), because of the race condition described        *
*        above.                                                                *
*                                                                              *
\******************************************************************************/


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int my_dup(int oldfd)
{
	return fcntl(oldfd, F_DUPFD, 0);
}

int my_dup2(int oldfd, int newfd)
{
	int rc = close(newfd);
	if ((rc == -1) && (errno != EBADF))
	{
		int saveErrno = errno;
		char msg_buffer[256];
		sprintf(msg_buffer, "dup2: error during close of %d", newfd);
		errno = saveErrno;
		perror(msg_buffer);
		return -1;
	}
	return fcntl(oldfd, F_DUPFD, newfd);
}

void desc_fd(int fd)
{
	struct stat statbuf;
	fstat(fd, &statbuf);
	printf("FD (%d) points to inode #%ld\n", fd, statbuf.st_ino);
}

int main(int argc, char *argv[])
{
	int fd1 = open("test.dat", O_RDONLY);
	int fd2 = open("test.lst", O_RDONLY);
	int fd3 = my_dup(fd1);
	int fd4 = my_dup(fd2);
	desc_fd(fd1);
	desc_fd(fd2);
	desc_fd(fd3);
	desc_fd(fd4);
	int fd5 = my_dup2(fd2, fd3);
	desc_fd(fd5);
    exit(0);
}
