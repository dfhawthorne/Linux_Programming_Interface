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
    
    /**************************************************************************\
    * Need to cater for '//', '/../', and '/./' within pathnames. These lead   *
    * the following edge conditions:                                           *
    * (1) "/"                                                                  *
    * (2) "/./"                                                                *
    * (3) "//"                                                                 *
    * (4) "/etc/.."                                                            *
    * (5) "/."                                                                 *
    * (6) "/etc/"                                                              *
    \**************************************************************************/
    
    for (char *cp = resolved_path; *cp != '\0'; ) {
        if (*cp++ != '/')
            continue;
        if (*cp == '\0')          /* Cannot look ahead */
            break;
        switch (*cp++) {
            case '/':               /* Found '//' */
                strcpy(cp-1, cp);   /* Eliminate extra '/' */
                break;
            case '.':               /* Found '/.' */
                switch (*cp++) {
                    case '/':       /* Found '/./' */
                    case '\0':
                        strcpy(cp-2, cp);
                        break;
                    case '.':       /* Found '/..' */
                        if (*cp != '/' && *cp != '\0')
                            break;
                        /* Found '/../' */
                        char *dp;
                        for (dp = cp - 5; *dp != '/'; --dp)
                            ;
                        strcpy(dp + 1, cp);
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
    
    /* Remove trailing '/', if any */
    
    int len = strlen(resolved_path);
    if (resolved_path[len-1] == '/' && len > 1)
        resolved_path[len-1] = '\0';
    
    return resolved_path;
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