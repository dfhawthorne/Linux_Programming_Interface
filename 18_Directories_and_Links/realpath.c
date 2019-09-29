/********************************************************************************
* Exercise 18-3: Implement realpath()                                           *
*                                                                               *
* The realpath() library function derefences all symbolic links in pathname (a  *
* null-terminated string) and resolves all references to /. and /.. to produce  *
* null-terminated string containing the corresponding absolute pathname.        *
********************************************************************************/

#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "tlpi_hdr.h"


#define BUF_SIZE PATH_MAX


/********************************************************************************
* char *realpath(const char *pathname, char *resolved_path);                    *
*     returns pointer to resolved pathname on success, or NULL on error.        *
********************************************************************************/

char *realpath(const char *pathname, char *resolved_path)
{
    struct stat statbuf;
    ssize_t numBytes;
    char buf[BUF_SIZE],
        cwd[BUF_SIZE];
    
    /*
     * Walks the path name back up one (1) level, stopping at the root.
     */
    
    char *goUpOneLevel(char *cp, char *resolved_path)
    {
        char *dp;
        
        for (dp = cp - 1; dp > resolved_path && *dp != '/'; dp--)
            ;
        
        if (dp > resolved_path)
        {
            return dp;
        } else {
            return resolved_path;
        }
    }   /* goUpOneLevel */
    
    if ((lstat(pathname, &statbuf) == -1) && (errno != ENOENT))
        return NULL;
    
    if ((errno != ENOENT) && (S_ISLNK(statbuf.st_mode))) {
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
    
    char *lp = NULL,                /* Look-ahead pointer */
        *cp = NULL;                 /* Current pointer */
    
    for (cp = resolved_path; *cp != '\0'; ) {
        lp = cp;
        if (*lp++ != '/') {
            cp++;
            continue;
        }
        if (*lp == '\0')            /* Cannot look ahead */
            break;
        switch (*lp++) {
            case '/':               /* Found '//' */
                strcpy(cp+1, lp);   /* Eliminate extra '/' */
                break;
            case '.':               /* Found '/.' */
                switch (*lp++) {
                    case '\0':      /* Founf '/.\0' */
                        *++cp = '\0';
                        break;
                    case '/':       /* Found '/./' */
                        strcpy(cp+1, lp);
                        break;
                    case '.':       /* Found '/..' */
                        switch (*lp) {
                            case '\0':  /* Found '/..\0' */
                                cp = goUpOneLevel(cp, resolved_path);
                                *++cp = '\0';
                                break;
                            case '/':  /* Found '/../' */
                                cp = goUpOneLevel(cp, resolved_path);
                                strcpy(cp+1, lp+1);
                                break;
                            default:
                                cp++;
                                break;
                        }
                        break;
                    default:
                        cp++;
                        break;
                }
                break;
            default:
                cp++;
                break;
        }
        /*
         * Check if current position contains a NULL char after any string
         * munging
         */
        if (*cp == '\0')
            break;
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

    if (realpath(argv[1], buf) == NULL)
        errExit("realpath");
    printf("realpath: %s --> %s\n", argv[1], buf);

    exit(EXIT_SUCCESS);
}