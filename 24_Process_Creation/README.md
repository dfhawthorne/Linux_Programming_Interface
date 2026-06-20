# 24 Process Creation

## Exercise 24-1

Run:

```bash
make test_1
```

The sample output is:

```text
There are eight processes created:
Parent executes fork()
Parent and child_1 execute fork()
Parent, child_1, child_2, and child_1_1 execute fork()
Now Parent, child_1, child_2, child_3, child_1_1, child_1_2, child_2_1, and child_1_1_1 are executing.

The process tree is:
Parent +-> child_1 +-> child_1_1 -> child_1_1_1
       |           +-> child_1_2
       +-> child_2  -> child_2_1
       +-> child_3
```

## Exercise 24-2

Run:

```bash
make test_2 VERBOSE=1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -O2 Ex_2.c -o Ex_2
./Ex_2 -v
Parent: Check status of FD 0 before vfork()
Parent: Status of FD 0 before vfork() successful
Parent: Before vfork()
Child: Close FD 0
Child: Exiting
Parent: Check status of FD 0 after child closes FD 0
Parent: Status of FD 0 after vfork() successful
```

## Exercise 24-3

Run:

```bash
make test_3 VERBOSE=1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_3.c -o Ex_3
ulimit -c unlimited
./Ex_3 -v
Parent: Before vfork()
Child: Get process ID
Child: Signal QUIT to get a core dump
Parent: vfork() successful
```
