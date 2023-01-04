/* -------------------------------------------------------------------------- 
 * Simplified version of setfattr:
 *
 * Ref: https://man7.org/linux/man-pages/man1/setfattr.1.html
 * 
 * setfattr [-h] -n name [-v value] pathname...
 * setfattr [-h] -x name pathname...
 * -------------------------------------------------------------------------- */

#define MAX_VALUE_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <unistd.h>

void usage(char *pgm_name, char *msg) {
    fprintf(stderr, "%s setfattr [-h] -n name [-v value] pathname...\n", pgm_name);
    fprintf(stderr, "%s setfattr [-h] -x name pathname...\n", pgm_name);
    if (msg != NULL) {
        fprintf(stderr, "%s\n", msg);
    }
}

int main(int argc, char **argv) {
    enum operations {noop, set_parm, show_parm, remove_parm};
    char *pgmname = argv[0];
    char *pathname = NULL;
    char *parm_name = NULL;
    char *parm_value = NULL;
    char *operation_names[] = {"Not set", "Set parm", "Show parm", "Remove parm"};
    enum operations operation = noop;
    int opt;
    int verbosity = 0;

    while ((opt = getopt(argc, argv, ":hn:v:x:")) != -1) {
        switch ((char)opt) {
            case 'h':
                usage(pgmname, NULL);
                exit(EXIT_SUCCESS);
                break;
            case 'n':
                parm_name = optarg;
                operation = show_parm;
                break;
            case 'v':
                parm_value = optarg;
                operation = set_parm;
                break;
            case 'x':
                parm_name = optarg;
                operation = remove_parm;
                break;
            case ':':
                usage(pgmname, "Missing parameter value");
                exit(EXIT_FAILURE);
                break;
            default:
                usage(pgmname, "Invalid parameter");
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (verbosity) {
        fprintf(stderr, "parm_name='%s'\n", (parm_name) ? parm_name : "MISSING");
        fprintf(stderr, "parm_value='%s'\n", (parm_value) ? parm_value : "MISSING");
        fprintf(stderr, "operation='%s'\n", operation_names[operation]);
    }

    if (optind >= argc) {
        usage(pgmname, "expecting path name");
        exit(EXIT_FAILURE);
    }

    pathname = argv[optind];

    switch (operation) {
        case noop:
            usage(pgmname, "No operation specified");
            exit(EXIT_FAILURE);
            break;
        case set_parm:
            if (setxattr(pathname, parm_name, parm_value, strlen(parm_value),0)) {
                fprintf(stderr, "Set parameter failed: %m\n");
                exit(EXIT_FAILURE);
            }
            break;
        case show_parm:
            parm_value = (char *)malloc(MAX_VALUE_SIZE);
            if (parm_value) {
                ssize_t parm_value_len = 0;
                
                parm_value_len = getxattr(pathname, parm_name, parm_value, (size_t)MAX_VALUE_SIZE);
                if (parm_value_len == -1) {
                    fprintf(stderr, "Get parameter failed: %m\n");
                    exit(EXIT_FAILURE);
                } else {
                    printf("%s='%*s'\n", parm_name, (int)parm_value_len, parm_value);
                }
            } else {
                fprintf(stderr, "%s: unable to allocate storage: %m\n", pgmname);
                exit(EXIT_FAILURE);
            }
            break;
        case remove_parm:
            if (removexattr(pathname, parm_name)) {
                fprintf(stderr, "Remove parameter failed: %m\n");
                exit(EXIT_FAILURE);
            }
            break;
    }

    exit(EXIT_SUCCESS);
}