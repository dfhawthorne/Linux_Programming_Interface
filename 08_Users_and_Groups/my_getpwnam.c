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
#include <string.h>
#include <sys/types.h>

/******************************************************************************\
* My implementation of getpwnam()                                              *
\******************************************************************************/

struct passwd *my_getpwnam(const char *name)
{
    struct passwd *pwd;
    
    while ((pwd = getpwent()) != NULL)
    {
        if (strcmp(pwd -> pw_name, name) == 0)
            break;
    }
    
    if (pwd != NULL)
    {
        ssize_t len = sizeof(struct passwd) +
            strlen(pwd -> pw_name)   + 1 +
            strlen(pwd -> pw_passwd) + 1 +
            strlen(pwd -> pw_gecos)  + 1 +
            strlen(pwd -> pw_dir)    + 1 +
            strlen(pwd -> pw_shell)  + 1;
        void *result = malloc(len);
        if (result == NULL)
        {
            endpwent();  
            errno = ENOMEM;
            return NULL;
        }
        memcpy(result, (void *)pwd, sizeof(struct passwd));
        char *ptr = (char *)result + sizeof(struct passwd);
        struct passwd *new_pwd = (struct passwd *)ptr;
        strcpy(ptr, pwd -> pw_name);
        new_pwd -> pw_name     = ptr;
        ptr += strlen(pwd -> pw_name)   + 1;
        strcpy(ptr, pwd -> pw_passwd);
        new_pwd -> pw_passwd   = ptr;
        ptr += strlen(pwd -> pw_passwd) + 1;
        strcpy(ptr, pwd -> pw_gecos);
        new_pwd -> pw_gecos    = ptr;
        ptr += strlen(pwd -> pw_gecos)  + 1;
        strcpy(ptr, pwd -> pw_dir);
        new_pwd -> pw_dir      = ptr;
        ptr += strlen(pwd -> pw_dir)    + 1;
        strcpy(ptr, pwd -> pw_shell);
        new_pwd -> pw_shell    = ptr;
        endpwent();
        errno                  = 0;
        return result;
    }
    else
    {
        endpwent();  
        errno = ENOENT;
        return NULL;
    }
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char *argv[])
{
    char *user = (argc < 2) ? getenv("USERNAME") : argv[1];
    struct passwd
        *my_pwd   = my_getpwnam(user),
        *real_pwd = getpwnam(user);
    
    if (my_pwd != NULL && real_pwd != NULL)
    {    
        printf("Login name (%s) %s\n",     real_pwd -> pw_name,  (strcmp(real_pwd -> pw_name,   my_pwd -> pw_name)   ? "mismatch" : "match"));
        printf("Password %s\n",                                  (strcmp(real_pwd -> pw_passwd, my_pwd -> pw_passwd) ? "mismatch" : "match"));
        printf("User ID (%d) %s\n",        real_pwd -> pw_uid,   (real_pwd -> pw_uid != my_pwd -> pw_uid             ? "mismatch" : "match"));
        printf("Group ID (%d) %s\n",       real_pwd -> pw_gid,   (real_pwd -> pw_gid != my_pwd -> pw_gid             ? "mismatch" : "match"));
        printf("User info (%s) %s\n",      real_pwd -> pw_gecos, (strcmp(real_pwd -> pw_gecos,  my_pwd -> pw_gecos)  ? "mismatch" : "match"));
        printf("Home directory (%s) %s\n", real_pwd -> pw_dir,   (strcmp(real_pwd -> pw_dir,    my_pwd -> pw_dir)    ? "mismatch" : "match"));
        printf("Shell (%s) %s\n",          real_pwd -> pw_shell, (strcmp(real_pwd -> pw_shell,  my_pwd -> pw_shell)  ? "mismatch" : "match"));
    }
    else
    {
        fprintf(stderr, "user (%s) not found\n", user);
        exit(1);
    }
    exit(0);
}
