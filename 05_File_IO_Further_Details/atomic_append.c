/******************************************************************************\
* Exercise 5-3: Opens a file with or without O_APPEND, and writes a specified  *
* number of bytes (one byte at a time).                                        *
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

	if (argc < 3)
	{
		fprintf(stderr, "%s needs number of bytes to write\n", argv[0]);
		exit(1);
	}
	
	long int num_bytes_to_write = strtol(argv[2], NULL, 10);

	int open_mode = O_CREAT | O_WRONLY;
	mode_t access = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	int append_mode  = 1;
	
	if ((argc > 3) && (*argv[3] == 'x'))
	{
		append_mode   = 0;
	} else {
		open_mode    |= O_APPEND;
		append_mode   = 1;
	}
	
	int fd = open(argv[1], open_mode, access);
	if (fd == -1)
	{
		perror("Unable to open file");
		exit(1);
	}
	
	const char *buffer = " ";
	for (long int i = 0; i < num_bytes_to_write; i++)
	{
		if (!append_mode)
		{
			lseek(fd, 0, SEEK_END);
		}
		ssize_t num_written = write(fd, buffer, 1);
		if (num_written == -1)
		{
			perror("Unable to write to file");
			close(fd);
			exit(1);
		}
	}
	
	close(fd);

	exit(0);
}
