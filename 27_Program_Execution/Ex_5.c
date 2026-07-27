// -----------------------------------------------------------------------------
// Exercise 27-5:
//
// When we run the following program, we find it produces no output. Why is
// this?
// -----------------------------------------------------------------------------

#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    int verbose = 0;
    int buffering = 1;

    while ((opt = getopt(argc, argv, "nv")) != -1) {
        switch (opt) {
            case 'n':
                buffering = 0;
                break;
            case 'v':
                verbose = 1;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-v] [-n]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }

    if (verbose) fprintf(stderr, "%s: verbose logging enabled\n", argv[0]);

    if (! buffering) {
        if (verbose) fprintf(stderr, "%s: disabled buffering on stdout\n", argv[0]);
        setbuf(stdout, NULL);
    }

    printf("Hello World");
    if (verbose) fprintf(stderr, "%s: execlp called\n", argv[0]);
    execlp("sleep", "sleep", "0", (char *) NULL);
}
