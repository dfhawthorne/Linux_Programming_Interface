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
    char buf[BUF_SIZE],
        cwd[BUF_SIZE];
    
    if (lstat(pathname, &statbuf) == -1)
        return NULL;
    
    if (S_ISLNK(statbuf.st_mode)) {
        numBytes = readlink(pathname, buf, BUF_SIZE - 1);
        if (numBytes == -1)
            return NULL;
        buf[numBytes] = '\0';          /* Add terminating null byte */
    } else {
        strcpy(buf, pathname);
    }
    
    if (buf[0] != '/') {                /* Is it a relative path name ? */
        if (getcwd(cwd, BUF_SIZE) == NULL)
            return NULL;
        strcpy(resolved_path, cwd);
        resolved_path[strlen(cwd)] = '/';
        strcpy(resolved_path + strlen(cwd) + 1, buf);
    } else {
        strcpy(resolved_path, buf);
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

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    if (lstat(argv[1], &statbuf) == -1)
        errExit("lstat");

    if (realpath(argv[1], buf) == NULL)
        errExit("realpath");
    printf("realpath: %s --> %s\n", argv[1], buf);

    exit(EXIT_SUCCESS);
}