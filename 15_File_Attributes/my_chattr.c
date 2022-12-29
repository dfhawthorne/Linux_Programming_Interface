/* -------------------------------------------------------------------------- *
 * Exercise 15-7: Implement chattr(1) command except for -R, -V, and -v       *
 *                options.                                                    *
 *                                                                            *
 * `man 1 chattr` says:                                                       *
 * SYNOPSIS                                                                   *
 *        chattr [ -RVf ] [ -v version ] [ -p project ] [ mode ] files...     *
 *                                                                            *
 * DESCRIPTION                                                                *
 *        chattr changes the file attributes on a Linux file system.          *
 *                                                                            *
 *        The format of a symbolic mode is +-=[aAcCdDeFijPsStTu].             *
 *                                                                            *
 *        The  operator  '+'  causes the selected attributes to be added to   *
 *        the existing attributes of the files; '-' causes them to  be  re‐   *
 *        moved;  and  '='  causes  them to be the only attributes that the   *
 *        files have.                                                         *
 *                                                                            *
 *        The letters 'aAcCdDeFijPsStTu' select the new attributes for  the   *
 *        files:  append only (a), no atime updates (A), compressed (c), no   *
 *        copy on write (C), no dump  (d),  synchronous  directory  updates   *
 *        (D),  extent  format (e), case-insensitive directory lookups (F),   *
 *        immutable (i), data journalling (j), project hierarchy  (P),  se‐   *
 *        cure  deletion (s), synchronous updates (S), no tail-merging (t),   *
 *        top of directory hierarchy (T), and undeletable (u).                *
 *                                                                            *
 *        The following attributes are read-only,  and  may  be  listed  by   *
 *        lsattr(1)  but not modified by chattr: encrypted (E), indexed di‐   *
 *        rectory (I), inline data (N), and verity (V).                       *
 *                                                                            *
 *        Not all flags are supported or utilized by all filesystems; refer   *
 *        to  filesystem-specific  man pages such as btrfs(5), ext4(5), and   *
 *        xfs(5) for more filesystem-specific details.                        *
 *                                                                            *
 * OPTIONS                                                                    *
 *        -R     Recursively change attributes  of  directories  and  their   *
 *               contents.                                                    *
 *                                                                            * 
 *        -V     Be verbose with chattr's output and print the program ver‐   *
 *               sion.                                                        *
 *                                                                            *
 *        -f     Suppress most error messages.                                *
 *                                                                            *
 *        -v version                                                          *
 *               Set the file's version/generation number.                    *
 *                                                                            *
 *        -p project                                                          *
 *               Set the file's project number.                               *
 * -------------------------------------------------------------------------- */

#include <ctype.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

void usage(char *pgm_name, char *msg)
{
    fprintf(stderr,
        "Usage: %s %s\n",
            pgm_name,
            "[ -RVf ] [ -v version ] [ -p project ] [ mode ] files..."
            );
    if (msg != NULL)
    fprintf(stderr, "%s\n", msg);
}

int main(int argc, char *argv[])
{
    int opt;
    int args_done = 0;
    int verbosity = 0;
    int suppress_errors = 0;
    int file_version = 0;
    int project_num = 0;
    char *mode = NULL;
    char *file_name = NULL;
    char *pgm_name = argv[0];
    
    while (!args_done && ((opt = getopt(argc, argv, ":RVfv:p:")) != -1))
    {
        if (verbosity > 1) {
            fprintf(stderr, "optind=%d, opt='%c',optarg='%s'\n", optind, opt, optarg);
        }
        switch (opt)
        {
            case 'R':
                usage(pgm_name, "-R is not implemented");
                exit(EXIT_FAILURE);
                break;
            case 'V':
                verbosity++;
                break;
            case 'f':
                suppress_errors = 1;
                break;
            case 'v':
                if (verbosity > 1) {
                    fprintf(
                        stderr,
                        "set file version\nargv[%d]='%s'\n",
                        optind,
                        argv[optind]);
                }
                if (optarg != NULL && isdigit(*optarg)) {
                    file_version = atoi(optarg);
                } else {
                    usage(pgm_name, "-v requires a numeric argument");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'p':
                if (verbosity > 1) {
                    fprintf(
                        stderr,
                        "set project number\nargv[%d]='%s'\n",
                        optind,
                        argv[optind]);
                }
                if (optarg != NULL && isdigit(*optarg)) {
                    project_num = atoi(optarg);
                } else {
                    usage(pgm_name, "-p requires a numeric argument");
                    exit(EXIT_FAILURE);
                }
                break;
            case ':':
                fprintf(stderr, "Missing argument\n");
                if (verbosity > 1) {
                    fprintf(
                        stderr,
                        "argv[%d]='%s'\n",
                        optind,
                        argv[optind]);
                }
                exit(EXIT_FAILURE);
                break;
            default: /* '?' */
                if (verbosity > 1) {
                    fprintf(
                        stderr,
                        "end of arguments\nargv[%d]='%s'\n",
                        optind,
                        argv[optind]);
                }
                args_done = 1;
                break;
       }
    }
    
    optind -= (args_done) ? 1 : 0;
    if (optind < argc)
    {
        mode = argv[optind];
        switch (*mode)
        {
            case '+':
            case '-':
            case '=':
                if (optind + 1 >= argc)
                {
                    fprintf(stderr, "Expecting file names\n");
                    exit(EXIT_FAILURE);
                } else {
                    file_name = argv[optind+1];    
                }
                break;
            default:
                mode = NULL;
                file_name = argv[optind];
                break;
        }
    } else {
        fprintf(stderr, "Expecting file names\n");
        exit(EXIT_FAILURE);
    }


    if (verbosity)
    {
        fprintf(stderr, "Version: 1.0\n");
        fprintf(stderr,
            "Errors: %s\n",
            (suppress_errors) ? "suppressed" : "displayed"
            );
        fprintf(stderr, "File version: ");
        if (file_version)
        {
            fprintf(stderr, "%d\n", file_version);
        }
        else
        {
            fprintf(stderr, "not changed\n");
        }
        fprintf(stderr, "Project number: ");
        if (project_num)
        {
            fprintf(stderr, "%d\n", project_num);
        }
        else
        {
            fprintf(stderr, "not changed\n");
        }
        fprintf(stderr, "Mode: %s\n", (mode) ? mode : "Not used");
        fprintf(stderr, "File name: %s\n", file_name);
    }

    if (! mode) {
        fprintf(
            stderr,
            "%s: No mode specified\n",
            pgm_name
        );
        exit(EXIT_FAILURE);
    }

    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s: open of '%s' failed: %m\n", pgm_name, file_name);
        exit(EXIT_FAILURE);
    }

    enum fs_ops { set_flags, rm_flags, add_flags };
    enum fs_ops op;
    switch (*mode) {
        case '-':
            op = rm_flags;
            break;
        case '+':
            op = add_flags;
            break;
        case '=':
            op = set_flags;
            break;
        default:
            usage(pgm_name, "Invalid mode operation");
            exit(EXIT_FAILURE);
    }

    unsigned int fs_mask = 0;
    for ( char *flag_ch = mode + 1; *flag_ch; flag_ch++ ) {
        switch (*flag_ch) {
            case 'a': fs_mask |= FS_APPEND_FL;       break;
            case 'c': fs_mask |= FS_COMPR_FL;        break;
            case 'D': fs_mask |= FS_DIRSYNC_FL;      break;
            case 'i': fs_mask |= FS_IMMUTABLE_FL;    break;
            case 'j': fs_mask |= FS_JOURNAL_DATA_FL; break;
            case 'A': fs_mask |= FS_NOATIME_FL;      break;
            case 'd': fs_mask |= FS_NODUMP_FL;       break;
            case 't': fs_mask |= FS_NOTAIL_FL;       break;
            case 's': fs_mask |= FS_SECRM_FL;        break;
            case 'S': fs_mask |= FS_SYNC_FL;         break;
            case 'T': fs_mask |= FS_TOPDIR_FL;       break;
            case 'u': fs_mask |= FS_UNRM_FL;         break;
            default:
                if ( ! suppress_errors ) {
                    fprintf(
                        stderr,
                        "%s: invalid mode '%c' found - ignored\n",
                        pgm_name,
                        *flag_ch
                    );
                }
                break;
        }
    }

    unsigned int attr = 0;

    if (op != set_flags && ioctl(fd, FS_IOC_GETFLAGS, &attr) == -1) {
        fprintf(
            stderr,
            "%s: Get flags from '%s' failed: %m\n",
            pgm_name,
            file_name);
        exit(EXIT_FAILURE);
    }

    if (verbosity > 1) {
        fprintf(
            stderr,
            "op=%d,attr(before)=%0X,",
            op,
            attr);
    }
    if (op == set_flags) {
        attr = fs_mask;
    } else {
        attr |= fs_mask;
        if (op == rm_flags) {
            attr ^= fs_mask;
        }
    }
    if (verbosity > 1) {
        fprintf(
            stderr,
            "attr(after)=%0X,fs_mask=%0X\n",
            attr,
            fs_mask);
    }

    if (ioctl(fd, FS_IOC_SETFLAGS, &attr) == -1) {
        fprintf(
            stderr,
            "%s: Set flags from '%s' failed: %m\n",
            pgm_name,
            file_name);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

