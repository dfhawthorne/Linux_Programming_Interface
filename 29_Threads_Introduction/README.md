# Chapter 29: Threads: Introduction

## Exercise 29-1

Run:

```bash
make test_1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 -pthread -D_REENTRANT -D_GNU_SOURCE Ex_1.c -o Ex_1
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 -pthread -D_REENTRANT -D_GNU_SOURCE Ex_1a.c -o Ex_1a
./Ex_1.sh
Unamended program fails
./Ex_1:Ex_1.c:23: pthread_join() failed: Resource deadlock avoided
Amended program succeeds
```

## Exercise 29-2

Run:

```bash
make test_2
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 -pthread -D_REENTRANT -D_GNU_SOURCE Ex_2.c -o Ex_2
./Ex_2
start thread
main thread exiting...
threadFunc entered
threadFunc exited
```

The problem I see is that the returned value from the `threadFunc()` function is
never referenced. A possible solution would be to have a `pthread_join()` before
the exit in order to get the returned value.
