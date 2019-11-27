/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
*                                                                         *
* This code is based upon list_files.c.                                   *
\*************************************************************************/

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static void             /* List all processes */
listProcs(const uid_t uid)
{
    DIR *dirp;

    dirp = opendir("/proc");
    if (dirp  == NULL) {
        fprintf(stderr, "opendir failed on '/proc': %m\n");
        return;
    }

    /* For each entry in this directory, print directory + filename */

    for (;;) {
        struct dirent *dp;
        
        errno = 0;              /* To distinguish error from end-of-directory */
        if ((dp = readdir(dirp)) == NULL)
            break;

        if (isdigit(dp -> d_name[0]) == 0)
            continue;           /* Skip /proc entries that are not processes */

        printf("%s\n", dp->d_name);
    }

    if (errno != 0) {
        fprintf(stderr, "readdir: %m\n");
    }

    if (closedir(dirp) == -1)
        fprintf(stderr, "closedir: %m\n");
}

int
main(int argc, char *argv[])
{
    char *user = (argc < 2) ? getenv("USERNAME") : argv[1];
    struct passwd
        *real_pwd = getpwnam(user);

    listProcs(real_pwd -> pw_uid);

    exit(0);
}
