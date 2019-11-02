/******************************************************************************\
* Exercise 5-2: Opens an existing file with O_APPEND, seeks to the start, and  *
* writes some data.                                                            *
\******************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "%s needs a file name parameter\n", argv[0]);
		exit(1);
	}

	int fd = open(argv[1], O_RDWR | O_APPEND);
	if (fd == -1)
	{
		perror("Unable to open file");
		exit(1);
	}
	
	off_t new_offset = lseek(fd, 0, SEEK_SET);
	fprintf(stdout, "Now positioned at %ld\n", (long)new_offset);
	
	const char *buffer = "Exercise 5-2 completed";
	ssize_t num_written = write(fd, buffer, strlen(buffer));
	if (num_written == -1)
	{
		perror("Unable to write to file");
		close(fd);
		exit(1);
	}
	
	new_offset = lseek(fd, 0, SEEK_CUR);
	fprintf(stdout, "Now positioned at %ld\n", (long)new_offset);

	exit(0);
}
