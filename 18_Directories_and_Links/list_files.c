/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

#include <dirent.h>
#include <stddef.h>
#include "tlpi_hdr.h"

static void             /* List all files in directory 'dirpath' */
listFiles(const char *dirpath)
{
    DIR *dirp;
    struct dirent *dp = NULL;
    Boolean isCurrent;          /* True if 'dirpath' is "." */
    size_t len;
    
    len = offsetof(struct dirent, d_name) + NAME_MAX + 1;
    dp = malloc(len);
    if (dp == NULL)
        errExit("malloc");

    isCurrent = strcmp(dirpath, ".") == 0;

    dirp = opendir(dirpath);
    if (dirp  == NULL) {
        errMsg("opendir failed on '%s'", dirpath);
        return;
    }

    /* For each entry in this directory, print directory + filename */

    for (;;) {
        struct dirent *result;
        
        errno = 0;              /* To distinguish error from end-of-directory */
        if ((readdir_r(dirp, dp, &result) != 0) || (result == NULL))
            break;

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;           /* Skip . and .. */

        if (!isCurrent)
            printf("%s/", dirpath);
        printf("%s\n", dp->d_name);
    }

    if (errno != 0) {
        if (dp != NULL)
            free(dp);
        errExit("readdir");
    }
    
    if (dp != NULL)
        free(dp);

    if (closedir(dirp) == -1)
        errMsg("closedir");
}

int
main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [dir-path...]\n", argv[0]);

    if (argc == 1)              /* No arguments - use current directory */
        listFiles(".");
    else
        for (argv++; *argv; argv++)
            listFiles(*argv);

    exit(EXIT_SUCCESS);
}
