# Chapter 30: Threads: Thread Synchronisation

## Exercise 30-1

Run:

```bash
make test_1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 -pthread -D_REENTRANT -D_GNU_SOURCE -I../03_System_Programming_Concepts thread_incr.c -L../Shared_Libraries -ltlpi_base -o thread_incr 
./Ex_1.sh
Num of duplicate values found=595548
```
