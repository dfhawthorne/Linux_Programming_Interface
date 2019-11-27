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
#include <fcntl.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static void             /* List all processes */
listProcs(const uid_t uid)
{
    DIR *dirp;
    char status_fn[269];
    char buffer[1024];
    char process_name[1024];

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
        #ifdef DEBUG
        printf("PID=%s\n", dp->d_name);
        #endif
        sprintf(status_fn, "/proc/%s/status", dp->d_name);
        #ifdef DEBUG
        printf("File_name='%s'\n", status_fn);
        #endif
        FILE *fp = fopen(status_fn, "r");
        if (fp != NULL)
        {
            while (!feof(fp))
            {
                fgets(buffer, 1024, fp);
                if (strncmp(buffer, "Name:\t", 6) == 0)
                {
                    strncpy(process_name, buffer+6, 1022);
                    process_name[1022] = '\n';
                    process_name[1023] = '\0';
                }
                if (strncmp(buffer, "Uid:\t", 5) == 0)
                {
                    int process_uid;
                    sscanf(buffer, "Uid:\t%d ", &process_uid);
                    if (process_uid == (int)uid)
                    {
                        printf("%8s %s", dp->d_name, process_name);
                    }
                    break;
                }
            }
            fclose(fp);
        }
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
