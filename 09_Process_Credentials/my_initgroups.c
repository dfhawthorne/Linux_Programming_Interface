/******************************************************************************\
* Exercise 9-3: Implement initgroups()                                         *
* SYNOPSIS                                                                     *
*        int initgroups(const char *user, gid_t group);                        *
*                                                                              *
* DESCRIPTION                                                                  *
*        The  initgroups() function initializes the group access list by       *
*        reading the group database /etc/group and using all groups of which   *
*        user is a member.  The additional group group is also added to the    *
*        list.                                                                 *
*                                                                              *
*        The user argument must be non-NULL.                                   *
*                                                                              *
* RETURN VALUE                                                                 *
*        The initgroups() function returns 0 on success.  On error, -1 is      *
*        returned, and errno is set appropriately.                             *
*                                                                              *
* ERRORS                                                                       *
*        ENOMEM Insufficient memory to allocate group information structure.   *
*                                                                              *
*        EPERM  The calling process has insufficient privilege.  See the       *
*               underlying system call setgroups(2).                           *
\******************************************************************************/

#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/******************************************************************************\
* My implementation of initgroups()                                            *
\******************************************************************************/

int my_initgroups(const char *user, gid_t group)
{
    errno = EPERM;
    return -1;
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char *argv[])
{
    exit(0);
}
