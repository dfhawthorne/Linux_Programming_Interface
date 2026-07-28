# Chapter 29: Threads: Introduction

## Exercise 29-1

Run:

```bash
make test_1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 -pthread -D_REENTRANT -D_GNU_SOURCE Ex_1.c -o Ex_1
./Ex_1
./Ex_1:Ex_1.c:23: pthread_join() failed
make: *** [Makefile:32: test_1] Error 1
```
