/*******************************************************************************
 * access()  checks whether the calling process can access the file pathname. If
 * pathname is a symbolic link, it is dereferenced.
 *
 * The mode specifies the accessibility check(s) to be performed, and is either
 * the value F_OK, or a mask consisting of the bitwise OR of one or more of R_OK,
 * W_OK, and X_OK.  F_OK tests for the existence of the file. R_OK, W_OK, and
 * X_OK  test whether the file exists and grants read, write, and execute permis‐
 * sions, respectively.
 *
 * The check is done using the calling process's real UID and GID, rather than
 * the effective IDs as is done when actually attempting an operation (e.g.,
 * open(2)) on the file. Similarly, for the root user, the check uses the set of
 * permitted capabilities rather than the set of effective capabilities; and for
 * non-root users, the check uses an empty set of capabilities.
 ******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void print_help(char *msg, int rc)
{
    if (msg != NULL) fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "my_access -h\n");
    fprintf(stderr, "my_access -f file\n");
    fprintf(stderr, "my_access [-r] [-w] [-x] [-u uid] file\n");
    fprintf(stderr, "\t-h\tPrint this help message\n");
    fprintf(stderr, "\t-f\tTest for existence of file\n");
    fprintf(stderr, "\t-r\tTest whether effective user can read file\n");
    fprintf(stderr, "\t-w\tTest whether effective user can write file\n");
    fprintf(stderr, "\t-x\tTest whether effective user can execute file\n");
    fprintf(stderr, "\n\t-u\tEffective User ID to use\n");
    
    exit(rc);
}

int euid_access(const char *pathname, int mode, uid_t uid)
{
    int save_errno = 0;
    int res = 0;
    
    struct stat statbuf;
    
    res = access(pathname, mode);
    
    if (res == 0) return 0;
    
    save_errno = errno;
    
    if (stat(pathname, &statbuf) == -1) return -1;
    
    if (mode == F_OK) return 0; /* File exists */
 
    if ((uid == statbuf.st_uid) &&
        ((statbuf.st_mode & S_IRWXU) == mode << 8))
    {
        return 0; /* owner has required permissions */
    }

    errno = EACCES;
    return -1;
}

int
main(int argc, char *argv[])
{
    int fname;                  /* Location of filename argument in argv[] */
    uid_t euid;                 /* effective user-id */
    int opt;
    int f_mode = 0;             /* F_OK maps to zero (0) */
    int rwx_mode = 0;
    char *invalid_opt_msg_fmt = "'%c' is an invalid option";
    char *invalid_opt_msg;
    char *file_missing_msg = "file name is missing";
    char *conflict_args_msg = "specify either -f or a combination of -r -w -x";
    char *uid_msg = "Access for uid to '%s' using mask %d is %d (%m)\n";
    char *euid_msg = "Access for euid to '%s' using mask %d is %d (%m)\n";
    
    invalid_opt_msg = malloc(strlen(invalid_opt_msg_fmt));
    while ((opt = getopt(argc, argv, ":hfru:wx")) != -1)
    {
        switch (opt)
        {
            case 'f': f_mode = 1; break;
            case 'h': print_help(NULL, EXIT_SUCCESS); break;
            case 'r': rwx_mode |= R_OK; break;
            case 'w': rwx_mode |= W_OK; break;
            case 'x': rwx_mode |= X_OK; break;
            case 'u': euid = atoi(optarg); break;
            case '?':
                sprintf(invalid_opt_msg, invalid_opt_msg_fmt, (char)optopt);
                print_help(invalid_opt_msg, EXIT_FAILURE);
                break;
        }
    }
    
    fname = optind;

    if (fname >= argc) print_help(file_missing_msg, EXIT_FAILURE);
    if (f_mode == 0)
    {
        if (rwx_mode == 0) print_help(conflict_args_msg, EXIT_FAILURE);
    }
    else
    {
        if (rwx_mode != 0) print_help(conflict_args_msg, EXIT_FAILURE);
    }

    int mode = (f_mode) ? F_OK : rwx_mode;
    int res = 0;

    res = access(argv[fname], mode);
    printf(uid_msg, argv[fname], mode, res);

    if (seteuid(euid) != 0)
    {
        fprintf(stderr, "seteuid(%d) failed: %m\n", euid);
        exit(EXIT_FAILURE);
    }

    res = euid_access(argv[fname], mode, euid);
    printf(euid_msg, argv[fname], mode, res);
    
    exit(EXIT_SUCCESS);
}
