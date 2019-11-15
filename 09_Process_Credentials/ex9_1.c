/******************************************************************************\
* Exercise 9-1:                                                                *
\******************************************************************************/

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fsuid.h>
#include <sys/types.h>
#include <unistd.h>

#define NEW_UID_1 1001
#define NEW_UID_2 1002

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/

int main(int argc, char **argv)
{
    uid_t ruid = NEW_UID_1,
          euid = NEW_UID_1,
          suid = NEW_UID_2;
          
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

    /* Run each part of exercise 9-1 separately */
    
    switch (option)
    {
        case 'a':
            if (setuid(ruid) != 0)
            {
                fprintf(stderr, "setuid(%d) failed: %m", ruid);
                exit(1);
            }
            printf("UIDs after setuid(%d):\n", ruid);
            break;
            
        case 'b':
            if (setreuid(-1, euid) != 0)
            {
                fprintf(stderr, "setreuid(-1, %d) failed: %m", euid);
                exit(1);
            }
            printf("UIDs after setreuid(-1, %d):\n", euid);
            break;
            
         case 'c':
            if (seteuid(euid) != 0)
            {
                fprintf(stderr, "seteuid(%d) failed: %m", euid);
                exit(1);
            }
            printf("UIDs after seteuid(%d):\n", euid);
            break;
            
        case 'd':
            if (setfsuid(euid) != 0)
            {
                fprintf(stderr, "setfsuid(%d) failed: %m", euid);
                exit(1);
            }
            printf("UIDs after setfsuid(%d):\n", euid);
            break;
            
        case 'e':
            if (setresuid((uid_t)-1, euid, suid) != 0)
            {
                fprintf(stderr, "setresuid(-1, %d, %d) failed: %m", euid, suid);
                exit(1);
            }
            printf("UIDs after setresuid(-1, %d, %d):\n", euid, suid);
            break;
            
       default:
            fprintf(stderr, "Select an option from 'a' to 'e' (inclusive)\n");
            exit(1);
            break;
    }
    
    /* Display settings after changing UIDs */
    
    rc = system(cmd);
    if (rc != 0)
    {
        fprintf(stderr, "Retrieval of process UIDs failed (rc=%d)\n", rc);
    }
    exit(0);
}

