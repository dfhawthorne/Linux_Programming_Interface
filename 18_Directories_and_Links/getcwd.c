/******************************************************************************\
* Exercise 18-5: Implement a function that performs the equivalent of getcwd().*
*                                                                              *
* From the MAN page:                                                           *
*                                                                              *
* NAME                                                                         *
*        getcwd, getwd, get_current_dir_name - get current working directory   *
*                                                                              *
* SYNOPSIS                                                                     *
*        char *getcwd(char *buf, size_t size);                                 *
*                                                                              *
* DESCRIPTION                                                                  *
*       These  functions return a null-terminated string containing an         *
*       absolute pathname that is the current working directory of the calling *
*       process.  The pathname is returned as the function result and via the  *
*       argument buf, if present.                                              *
*                                                                              *
*       If the current directory is not below the root directory of the        *
*       current process (e.g., because the process set a new filesystem root   *
*       using chroot(2) without  changing  its  current  directory into  the   *
*       new  root),  then, since Linux 2.6.36, the returned path will be       *
*       prefixed with the string "(unreachable)".  Such behavior can also be   *
*       caused by an unprivileged user by changing the current directory into  *
*       another mount namespace.  When dealing with paths from untrusted       *
*       sources, callers of these functions should consider checking whether   *
*       the returned path starts with  '/' or '(' to avoid misinterpreting an  *
*       unreachable path as a relative path.                                   *
*                                                                              *
*       The getcwd() function copies an absolute pathname of the current       *
*       working directory to the array pointed to by buf, which is of length   *
*       size.                                                                  *
*                                                                              *
*       If the length of the absolute pathname of the current working          *
*       directory, including the terminating null byte, exceeds size bytes,    *
*       NULL is returned, and errno is set to ERANGE; an application should    *
*       check for this error, and allocate a larger buffer if necessary.       *
*                                                                              *
*       As an extension to the POSIX.1-2001 standard, glibc's getcwd()         *
*       allocates the buffer dynamically using malloc(3) if buf is NULL.  In   *
*       this case, the allocated buffer has the length size unless size is     *
*       zero, when buf is allocated as big as necessary.  The caller should    *
*       free(3) the returned buffer.                                           *
*                                                                              *
* RETURN VALUE                                                                 *
*       On success, these functions return a pointer to a string containing    *
*       the pathname of the current working directory.  In the case getcwd()   *
*       this is the same value as buf.                                         *
\******************************************************************************/

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/******************************************************************************\
* My implementation of getcwd                                                  *
\******************************************************************************/

char *getcwd(char *buf, size_t size)
{
    int cwdFD;                  /* Current working directory file descriptor  */
    struct stat cwdStat;        /* Information about current directory        */
    
    #ifdef DEBUG
    fprintf(stderr, "getcwd started\n");
    #endif

    /**************************************************************************\
    * Returns name matching device and i-node                                  *
    \**************************************************************************/
    
    char *getParentName(const dev_t device, const ino_t inode)
    {
        DIR *dir;               /* Parent directory                           */
        struct dirent *entry;   /* current entry in parent sirectory          */
        char *fileName;         /* name of parent                             */
        struct stat parentStat; /* Information about parent directory         */
        
        #ifdef DEBUG
        fprintf(stderr, "getParentName started: inode=%ld\n", (long)inode);
        #endif
        
        /**********************************************************************\
        * Get information about parent directory                               *
        * If the device and inode match, then we have reached the root.        *
        \**********************************************************************/
        
        int statRC;
        if ((statRC = stat("..", &parentStat)) == 0) {
            if (( parentStat.st_dev == device ) &&
                ( parentStat.st_ino == inode  )) {
                #ifdef DEBUG
                fprintf(stderr, "getParentName: reached root\n");
                #endif
                char *result = malloc(PATH_MAX + 1);
                if (result == NULL) {
                    #ifdef DEBUG
                    int saveErrno = errno;
                    perror("getParentName: malloc of result failed");
                    errno = saveErrno;
                    #endif
                    return NULL;
                }
                result[0] = '\0';
                return result;
            }
        } else {
            #ifdef DEBUG
            int saveErrno = errno;
            fprintf(stderr, "getParentName: stat of parent directory returned %d\n", statRC);
            perror("getParentName: stat of parent directory failed");
            errno = saveErrno;
            #endif
            return NULL;
        }
        
        /**********************************************************************\
        * Find the matching inode in the parent directory.                     *
        \**********************************************************************/
        
        chdir("..");        /* Change to parent directory                     */
        
        dir = opendir(".");
        if (dir == NULL) {
            #ifdef DEBUG
            int saveErrno = errno;
            perror("getParentName: open parent directory failed");
            errno = saveErrno;
            #endif
            return NULL;
        }
        
        for (;;)
        {
            errno = 0;
            entry = readdir(dir);
            if (entry == NULL)
                break;
            
            #ifdef DEBUG
            fprintf(stderr, "getParentName: inode=%ld name='%s'\n", (long)(entry -> d_ino), entry -> d_name);
            #endif
            
            if (entry -> d_ino == inode) {
                int size = strlen(entry -> d_name) + 1;
            
                #ifdef DEBUG
                fprintf(stderr, "getParentName: MATCH inode=%ld name='%s' size=%d\n", (long)(entry -> d_ino), entry -> d_name, size);
                #endif
                
                fileName = (char *)malloc(size);
                if (fileName != NULL) {
                    strcpy(fileName, entry -> d_name);
                #ifdef DEBUG
                } else {
                    int saveErrno = errno;
                    perror("getParentName: malloc for parent directory name failed");
                    errno = saveErrno;
                #endif
                }

                break;
            }
        }
        
        #ifdef DEBUG
        fprintf(stderr, "getParentName: About to close dir\n");
        #endif
        
        int saveErrno = errno;
        closedir(dir);
        
        #ifdef DEBUG
        fprintf(stderr,
                "getParentName closed directory: errno=%d fileName='%s'\n",
                saveErrno,
                (fileName == NULL) ? "(NULL)" : fileName
               );
        #endif

        char *currentWorkingDir = getParentName(parentStat.st_dev, parentStat.st_ino);
        if (currentWorkingDir == NULL) {
            #ifdef DEBUG
            saveErrno = errno;
            perror("getParentName called failed");
            errno = saveErrno;
            #endif
            return NULL;
        }
        
        int offset = strlen(currentWorkingDir);
        currentWorkingDir[offset++] = '/';
        strcpy(currentWorkingDir+offset, fileName);
        
        #ifdef DEBUG
        fprintf(stderr,
                "getParentName ended: errno=%d fileName='%s' cwd='%s'\n",
                saveErrno,
                (fileName == NULL) ? "(NULL)" : fileName,
                (currentWorkingDir == NULL) ? "NULL" : currentWorkingDir
               );
        #endif

        errno = saveErrno;
        
        return currentWorkingDir;
    }   /* getParentName */
    
    
    cwdFD = open(".", O_RDONLY);    /* Remember we are */
    if (cwdFD == -1)
        return NULL;
    fstat(cwdFD, &cwdStat);         /* Always succeeds */
    
    /**************************************************************************\
    * Implement an extension to the POSIX.1-2001 standard, glibc's getcwd()    *
    * allocates the buffer dynamically using malloc(3) if buf is NULL.  In     *
    * this case, the allocated buffer has the length size unless size is       *
    * zero, when buf is allocated as big as necessary.                         *
    \**************************************************************************/
    
    char *result = NULL;
    char *cwdStr = getParentName( cwdStat.st_dev, cwdStat.st_ino );
    
    if (cwdStr != NULL) {
        if (strlen(cwdStr) > size) {
            free(cwdStr);
            errno = ERANGE;
            result = NULL;
        } else {
            if (buf == NULL)
                buf = (char *) malloc( (size == 0) ? strlen(cwdStr)+1 : size );
            if (buf != NULL)
                strcpy(buf, cwdStr);
            free(cwdStr);
            result = buf;
        }
    } else {
        result = NULL;
    }
    
    int saveErrno = errno;
    
    fchdir(cwdFD);      /* Return to original directory */
    close(cwdFD);
    errno = saveErrno;
    
    return result;
}   /* getcwd */

/******************************************************************************\
* Driver routine to test getcwd                                                *
\******************************************************************************/

int main()
{
    char *buf = NULL,
        *ans = NULL;
    size_t size = 20;
    
    if (getcwd(buf, size) == NULL)
    {
        perror("Error in too small getcwd with null buffer");
        if (buf != NULL)
            free(buf);
    }
    
    size = PATH_MAX + 1;
    if ((ans = getcwd(buf, size)) == NULL)
    {
        perror("Error in max size getcwd with null buffer");
        if (buf != NULL)
            free(buf);
    } else {
        printf("getcwd successful with null buffer: cwd -> %s\n", ans);
        free(ans);
    }
    
    buf = malloc(size);
    if (buf == NULL) {
        perror("Error in malloc for buf");
        exit(1);
    }
    
    if (getcwd(buf, size) == NULL)
    {
        perror("Error in max size getcwd with pre-allocated buffer");
    } else {
        printf("getcwd successful with pre-allocated buffer: cwd -> %s\n", buf);
    }
    
    if (buf != NULL)
        free(buf);
    
    exit(0);
}