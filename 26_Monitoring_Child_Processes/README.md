# 26: Monitoring Child Processes

# Exercise 26-1

Run:

```bash
make test_1 VERBOSE=1
./Ex_1 -v
```

Sample output is:

```text
Parent: Create parent <-> child pipes
Parent: About to fork
Parent: about to exit
Child: close child -> parent pipe.
Child: waiting for parent to close pipe.
Child: Parent has closed parent -> child pipe
Child: Get process id of parent
Process ID of parent is now 1
```