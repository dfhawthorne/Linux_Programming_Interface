/********************************************************************************
* Exercise 18-3: Implement realpath()                                           *
*                                                                               *
* The realpath() library function derefences all symbolic links in pathname (a  *
* null-terminated string) and resolves all references to /. and /.. to produce  *
* null-terminated string containing the corresponding absolute pathname.        *
*                                                                               *
* char *realpath(const char *pathname, char *resolved_path);                    *
*     returns pointer to resolved pathname on success, or NULL on error.        *
********************************************************************************/

#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "tlpi_hdr.h"


#define BUF_SIZE PATH_MAX


char *realpath(const char *pathname, char *resolved_path)
{
    struct stat statbuf;
    ssize_t numBytes;
    
    resolved_path = malloc(PATH_MAX);
    if (resolved_path == NULL)
        return NULL;
    
    if (lstat(pathname, &statbuf) == -1)
        return NULL;
    
    if (S_ISLNK(statbuf.st_mode)) {
        numBytes = readlink(pathname, resolved_path, BUF_SIZE - 1);
        if (numBytes == -1)
            return NULL;
        resolved_path[numBytes] = '\0';          /* Add terminating null byte */
        return resolved_path;
    } else {
        return NULL; /* Not implemented yet */
    }
}

/*
 * Following code is based on view_symlink.c and is used to test my
 * realpath function.
 */

int
main(int argc, char *argv[])
{
    struct stat statbuf;
    char buf[BUF_SIZE];
    ssize_t numBytes;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    if (lstat(argv[1], &statbuf) == -1)
        errExit("lstat");

    numBytes = readlink(argv[1], buf, BUF_SIZE - 1);
    if (numBytes == -1)
        errExit("readlink");
    buf[numBytes] = '\0';                       /* Add terminating null byte */
    printf("readlink: %s --> %s\n", argv[1], buf);

    if (realpath(argv[1], buf) == NULL)
        errExit("realpath");
    printf("realpath: %s --> %s\n", argv[1], buf);

    exit(EXIT_SUCCESS);
}