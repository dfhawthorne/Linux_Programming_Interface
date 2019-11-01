/******************************************************************************\
* Exercise 5-2: Opens an existing file with O_APPEND, seeks to the start, and  *
* writes some data.                                                            *
\******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "%s needs a file name parameter\n", argv[0]);
		exit(1);
	}

	exit(0);
}
