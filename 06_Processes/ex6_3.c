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

#include <stdio.h>
#include <stdlib.h>

/******************************************************************************\
* My implementation of setenv                                                  *
\******************************************************************************/

int my_setenv(const char *name, const char *value, int overwrite)
{
    return setenv(name, value, overwrite);
}

/******************************************************************************\
* My implementation of unsetenv                                                *      
\******************************************************************************/

int my_unsetenv(const char *name)
{
    return unsetenv(name);
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

