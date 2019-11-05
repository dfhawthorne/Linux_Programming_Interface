/******************************************************************************\
* Exercise 6-3: Implement setenv() and unsetenv().                             *
*                                                                              *
* NAME                                                                         *
*        setenv - change or add an environment variable                        *
*                                                                              *
* SYNOPSIS                                                                     *
*        #include <stdlib.h>                                                   *
*                                                                              *
*        int setenv(const char *name, const char *value, int overwrite);       *
*                                                                              *
*        int unsetenv(const char *name);                                       *
*                                                                              *
* DESCRIPTION                                                                  *
*        The  setenv()  function  adds  the  variable name to the environment  *
*        with the value value, if name does not already exist.  If name does   *
*        exist in the environment, then its value is changed to value if       *
*        overwrite is nonzero; if overwrite is zero, then the value of name is *
*        not changed (and setenv() returns a success status).  This function   *
*        makes copies of the strings pointed to by name and value (by contrast *
*        with putenv(3)).                                                      *
*                                                                              *
*        The unsetenv() function deletes the variable name from the            *
*        environment.  If name does not exist in the environment, then the     *
*        function succeeds, and the environment is unchanged.                  *
*                                                                              *
* RETURN VALUE                                                                 *
*        The setenv() function returns zero on success, or -1 on error, with   *
*        errno set to indicate the cause of the error.                         *
*                                                                              *
*        The unsetenv() function returns zero on success, or -1 on error, with *
*        errno set to indicate the cause of the error.                         *
*                                                                              *
* ERRORS                                                                       *
*        EINVAL name is NULL, points to a string of length 0, or contains an   *
*        '=' character.                                                        *
*                                                                              *
*        ENOMEM Insufficient memory to add a new variable to the environment.  *
\******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

/******************************************************************************\
* My implementation of setenv                                                  *
\******************************************************************************/

int my_setenv(const char *name, const char *value, int overwrite)
{
    if (name == NULL || strlen(name) == 0 || strstr(name, "=") != NULL)
    {
        errno = EINVAL;
        return -1;
    }
    char *current_value = getenv(name);
    if (errno != 0) return -1;
    if (overwrite == 0 && current_value != NULL)
    {
        errno = 0;
        return 0;
    }
    char *environ_pair = malloc(strlen(name) + 1 + strlen(value) + 1);
    if (environ_pair == NULL)
    {
        errno = ENOMEM;
        return -1;
    }
    sprintf(environ_pair, "%s=%s", name, value);
    if (errno != 0)
    {
        int save_errno = errno;
        free(environ_pair);
        errno = save_errno;
        return -1;
    }
    int rc = putenv(environ_pair);
    return (rc == 0) ? 0 : -1;
    /* return setenv(name, value, overwrite); */
}

/******************************************************************************\
* My implementation of unsetenv                                                *      
\******************************************************************************/

int my_unsetenv(const char *name)
{
    if (name == NULL || strlen(name) == 0 || strstr(name, "=") != NULL)
    {
        errno = EINVAL;
        return -1;
    }
    
    #ifdef DEBUG
    fprintf(stderr, "my_unsetenv: name='%s'\n", name);
    #endif
    
    char **bp = NULL;
    int len = strlen(name);
    for (char **ep = environ; *ep != NULL; ep++)
    {
        #ifdef DEBUG
        fprintf(stderr, "my_unsetenv: ep=0X%016llX *ep=0X%016llX **ep='%s'\n", (long long int)ep, (long long int)*ep, (*ep == NULL) ? "NULL" : *ep);
        #endif
        if (strncmp(name, *ep, len) == 0 && (*ep)[len] == '=')
        {
            bp = ep;
            #ifdef DEBUG
            fprintf(stderr, "my_unsetenv: bp=0X%016llX *bp=0X%016llX **bp='%s'\n", (long long int)bp, (long long int)*bp, (*bp == NULL) ? "NULL" : *bp);
            fflush(stderr);
            #endif
            free(*bp);
            *bp = NULL;
        } else {
            if (bp != NULL)
            {
                *bp = *ep;
                bp++;
            }
        }
    }
    return 0;
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char *argv[])
{
    if (my_setenv("FRED", "NURK", 0) == -1)
        fprintf(stderr, "my_setenv(\"FRED\", \"NURK\", 0) failed: %m\n");
    char *fred = getenv("FRED");
    printf("FRED=%s\n", (fred != NULL) ? fred : "NULL");
    
    if (my_setenv("FRED", "charlie", 0) == -1)
        fprintf(stderr, "my_setenv(\"FRED\", \"charlie\", 0) failed: %m\n");
    fred = getenv("FRED");
    printf("FRED=%s\n", (fred != NULL) ? fred : "NULL");

    if (my_setenv("FRED", "charlie", 1) == -1)
        fprintf(stderr, "my_setenv(\"FRED\", \"charlie\", 1) failed: %m\n");
    fred = getenv("FRED");
    printf("FRED=%s\n", (fred != NULL) ? fred : "NULL");

    if (my_unsetenv("FRED") == -1)
        fprintf(stderr, "my_unsetenv(\"FRED\") failed: %m\n");
    fred = getenv("FRED");
    printf("FRED=%s\n", (fred != NULL) ? fred : "NULL");

    exit(0);
}

