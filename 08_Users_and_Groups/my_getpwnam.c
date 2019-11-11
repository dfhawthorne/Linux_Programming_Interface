/******************************************************************************\
* Exercise 8-2: Implement getpwnam().                                          *
* NAME                                                                         *
*        getpwnam - get password file entry                                    *
*                                                                              *
* SYNOPSIS                                                                     *
*        #include <sys/types.h>                                                *
*        #include <pwd.h>                                                      *
*                                                                              *
*        struct passwd *getpwnam(const char *name);                            *
*                                                                              *
* DESCRIPTION                                                                  *
*        The getpwnam() function returns a pointer to a structure containing   *
*        the broken-out fields of the record in the password database (e.g.,   *
*        the local password file /etc/passwd, NIS, and LDAP) that matches the  *
*        username name.                                                        *
*                                                                              * 
*        The passwd structure is defined in <pwd.h> as follows:                *
*                                                                              *
*            struct passwd {                                                   *
*                char   *pw_name;                                              *
*                char   *pw_passwd;                                            *
*                uid_t   pw_uid;                                               *
*                gid_t   pw_gid;                                               *
*                char   *pw_gecos;                                             *
*                char   *pw_dir;                                               *
*                char   *pw_shell;                                             *
*             };                                                               *
*                                                                              *
*        See passwd(5) for more information about these fields.                *
*                                                                              *
* RETURN VALUE                                                                 *
*        The getpwnam() function returns a pointer to a passwd structure, or   *
*        NULL if the matching entry is not found or an error occurs.  If an    *
*        error occurs, errno is set appropriately.                             *
*        If one wants to check errno after the call, it should be set to zero  *
*        before the call.                                                      *
*                                                                              *
*        The return value may point to a static area, and may be overwritten   *
*        by subsequent calls to getpwnam().  (Do not pass the returned pointer *
*        to free(3).)                                                          *
*                                                                              *
* ERRORS                                                                       *
*        0 or ENOENT or ESRCH or EBADF or EPERM or ...                         *
*               The given name or uid was not found.                           *
*                                                                              *
*        EINTR  A signal was caught; see signal(7).                            *
*                                                                              *
*        EIO    I/O error.                                                     *
*                                                                              *
*        EMFILE The per-process limit on the number of open file descriptors   *
*               has been reached.                                              *
*                                                                              *
*        ENFILE The system-wide limit on the total number of open files has    *
*               been reached.                                                  *
*                                                                              *
*        ENOMEM Insufficient memory to allocate passwd structure.              *
*                                                                              *
*        ERANGE Insufficient buffer space supplied.                            *
\******************************************************************************/

#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/******************************************************************************\
* My implementation of getpwnam()                                              *
\******************************************************************************/

struct passwd *my_getpwnam(const char *name)
{
    errno = ENOMEM;
    return NULL;
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char *argv[])
{
    exit(0);
}
