/******************************************************************************\
* Exercise 9-4:                                                                *
\******************************************************************************/

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fsuid.h>
#include <sys/types.h>
#include <unistd.h>

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char **argv)
{
    uid_t ruid,
          euid;
          
    char option = (argc > 1) ? argv[1][0] : '?';

    /* Display initial settings of UIDs */
    
    pid_t curr_pid = getpid();
    char cmd[255];
    sprintf(cmd, "grep -e \"^Uid:\" /proc/%d/status", (int) curr_pid);
    printf("Initial UIDs:\n");
    int rc = system(cmd);
    if (rc != 0)
    {
        fprintf(stderr, "Retrieval of process UIDs failed (rc=%d)\n", rc);
    }

    /* Run each part of exercise 9-4 separately */
    
    euid = geteuid();
    ruid = getuid();
    
    switch (option)
    {
        case 'a':
            if (seteuid(ruid) != 0)
            {
                fprintf(stderr, "seteuid(%d) failed: %m\n", ruid);
                exit(1);
            }
            printf("UIDs after seteuid(%d):\n", ruid);
            rc = system(cmd);
            if (rc != 0)
            {
                fprintf(stderr, "Retrieval of process UIDs failed (rc=%d)\n", rc);
            }
            if (seteuid(euid) != 0)
            {
                fprintf(stderr, "seteuid(%d) failed: %m\n", euid);
                exit(1);
            }
            printf("UIDs after seteuid(%d):\n", euid);
            rc = system(cmd);
            if (rc != 0)
            {
                fprintf(stderr, "Retrieval of process UIDs failed (rc=%d)\n", rc);
            }
            break;
            
        case 'b':
            if (setresuid(-1, ruid, ruid) != 0)
            {
                fprintf(stderr, "setresuid(-1, %d, %d) failed: %m\n", ruid, ruid);
                exit(1);
            }
            printf("UIDs after setresuid(-1, %d, %d):\n", ruid, ruid);
            rc = system(cmd);
            if (rc != 0)
            {
                fprintf(stderr, "Retrieval of process UIDs failed (rc=%d)\n", rc);
            }
            break;
            
       default:
            fprintf(stderr, "Select an option from 'a' to 'e' (inclusive)\n");
            exit(1);
            break;
    }

    exit(0);
}

