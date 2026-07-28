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
