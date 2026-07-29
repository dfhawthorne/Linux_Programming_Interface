// -----------------------------------------------------------------------------
// Exercise 29-2:
// -----------------------------------------------------------------------------

#include <error.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct someStruct {
    short len;
    char data[256];
    short opt;
} someStruct_t;

static void *threadFunc(void *arg) {
    fprintf(stderr, "threadFunc entered\n");
    someStruct_t *pbuf = (someStruct_t *)arg;
    char source_data[] = "Some data";

    pbuf -> len = strlen(source_data);
    strncpy(pbuf -> data, source_data, 256);
    pbuf -> opt = 0;
    fprintf(stderr, "threadFunc exited\n");
    return pbuf;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    someStruct_t buf;
    pthread_t thr;

    fprintf(stderr, "start thread\n");
    int line_num = __LINE__ + 1;
    int err_code = pthread_create(&thr, NULL, threadFunc, (void *)&buf);
    if (err_code)
        error_at_line(
            EXIT_FAILURE,
            err_code,
            __FILE__,
            line_num,
            "pthread_create() failed"
        );
    
    fprintf(stderr, "main thread exiting...\n");
    pthread_exit(NULL);
}
