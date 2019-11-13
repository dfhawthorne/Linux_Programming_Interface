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
#include <limits.h>
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
    if (user == NULL || strlen(user) == 0)
    {
        errno = EINVAL;
        return -1;
    }
    gid_t *grouplist = NULL;
    size_t size =(NGROUPS_MAX + 1) * sizeof(gid_t);
    if ((grouplist = (gid_t *)malloc(size)) == NULL)
    {
        errno = ENOMEM;
        return -1;
    }
    size_t gidsetsize       = 0;
    grouplist[gidsetsize++] = group;
    struct group *curr_group;
    while ((curr_group = getgrent()) != NULL)
    {
        for (char **member = curr_group -> gr_mem; *member != NULL; member++)
        {
            if (strcmp(*member, user) == 0)
            {
                grouplist[gidsetsize++] =  curr_group -> gr_gid;
                break;
            }
        }
    }
    endgrent();
    if (gidsetsize > 0)
    {
        int num_groups = setgroups(gidsetsize, grouplist);
        int save_errno = errno;
        free(grouplist);
        errno          = save_errno;
        return (num_groups == gidsetsize) ? 0 : -1;
    }
    
    free(grouplist);
    errno = EPERM;
    return -1;
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char *argv[])
{
    char *user = (argc < 2) ? getenv("USERNAME") : argv[1];
    int rc = my_initgroups(user, 0);
    if (rc != 0)
    {
        fprintf(stderr, "my_initgroups: %m\n");
        exit(1);
    }
    exit(0);
}
