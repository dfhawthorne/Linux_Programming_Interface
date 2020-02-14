/******************************************************************************\
* Exercise 12-3:                                                               *
* Write a program that lists all processes that have a particular file         *
* pathname open.                                                               *
*                                                                              *
* Parameters:                                                                  *
*     path name to search for                                                  *
\******************************************************************************/

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/******************************************************************************\
* Print all process IDs that have the file open                                *
\******************************************************************************/

void print_processes_with_file_open(const char * resolved_path)
{
    DIR *process_dirp;
    char proc_fd_dir_name[269];
    char buffer[1024];
    
    process_dirp = opendir("/proc");
    if (process_dirp  == NULL)
    {
        fprintf(stderr, "opendir failed on '/proc': %m\n");
        return;
    }

    /* ---------------------------------------------------------------------
       For each entry in the /proc directory, find a process sub-directory
       which is identified as starting with a digit. In that sub-directory,
       we want the 'fd' directory which has the link to the open file.
       --------------------------------------------------------------------- */

    for (;;)
    {
        struct dirent *pdp;
        
        errno = 0;              /* To distinguish error from end-of-directory */
        if ((pdp = readdir(process_dirp)) == NULL)
            break;

        if (isdigit(pdp -> d_name[0]) == 0)
            continue;           /* Skip /proc entries that are not processes */
        #ifdef DEBUG
        fprintf(stderr, "PID=%s\n", pdp->d_name);
        #endif
        sprintf(proc_fd_dir_name, "/proc/%s/fd", pdp->d_name);
        #ifdef DEBUG
        fprintf(stderr, "File_name='%s'\n", proc_fd_dir_name);
        #endif
        /* ------------------------------------------------------------------
           Get all file descriptors for this process ID
           ------------------------------------------------------------------ */
        DIR *fd_dirp = opendir(proc_fd_dir_name);
        if (fd_dirp != NULL)
        {
            for (;;)
            {
                struct dirent *fdp;
            
                errno = 0;          /* To distinguish error from end-of-directory */
                if ((fdp = readdir(fd_dirp)) == NULL)
                    break;
                
                if (isdigit(fdp -> d_name[0]) == 0)
                    continue;           /* Skip /proc entries that are not processes */
                #ifdef DEBUG
                fprintf(stderr, "FD=%s\n", fdp -> d_name);
                #endif
                
                ssize_t bytes_read = 0;
                const size_t max_bytes  = 268;
                char fd_link_name[max_bytes+1];
                char fd_path_name[max_bytes+1];
                snprintf(
                    fd_link_name,
                    max_bytes,
                    "%s/%s",
                    proc_fd_dir_name,
                    fdp -> d_name
                    );
                fd_link_name[max_bytes] = '\0';

                #ifdef DEBUG
                fprintf(stderr, "fd_link_name=%s\n", fd_link_name);
                #endif
                
                bytes_read = readlink(fd_link_name, fd_path_name, max_bytes);
                fd_path_name[bytes_read] = '\0';

                #ifdef DEBUG
                fprintf(stderr, "fd_path_name=%s\n", fd_path_name);
                #endif
                
                if (strcmp(fd_path_name, resolved_path) == 0)
                {
                    printf("PID: %s, FD: %s\n", pdp->d_name, fdp -> d_name);
                }
            }
        }
    }
}

/******************************************************************************\
* Main routine                                                                 *
\******************************************************************************/

int
main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "No path name entered\n");
        exit(1);
    }
    
    char *resolved_path = realpath(argv[1], NULL);
    if (resolved_path == NULL)
    {
        fprintf(stderr, "Supplied path (%s) cannot be resolved: %m\n", argv[1]);
        exit(1);
    }

    #ifdef DEBUG
    fprintf(stderr, "Resolved path for '%s' is '%s'\n", argv[1], resolved_path);
    #endif
    
    print_processes_with_file_open(resolved_path);
    
    free(resolved_path);
    exit(0);
}
