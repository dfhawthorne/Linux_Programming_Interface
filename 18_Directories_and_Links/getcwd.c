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
*       If  the length of the absolute pathname of the current working         *
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

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>

/******************************************************************************\
* My implementation of getcwd                                                  *
\******************************************************************************/

char *getcwd(char *buf, size_t size)
{
    errno = ERANGE;
    return NULL;
}   /* getcwd */

/******************************************************************************\
* Driver routine to test getcwd                                                *
\******************************************************************************/

int main()
{
    char *buf = NULL;
    size_t size = 20;
    
    if (getcwd(buf, size) == NULL)
    {
        perror("Error in too small getcwd with null buffer");
        if (buf != NULL)
            free(buf);
    }
    
    size = NAME_MAX + 1;
    if (getcwd(buf, size) == NULL)
    {
        perror("Error in max size getcwd with null buffer");
        if (buf != NULL)
            free(buf);
    } else {
        printf("getcwd successful with null buffer: cwd -> %s\n", buf);
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