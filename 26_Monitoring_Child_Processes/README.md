# 26: Monitoring Child Processes

## Exercise 26-1

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

## Exercise 26-2

Run:

```bash
make test_2 VERBOSE=1
```

Sample output is"

```text
Grandparent: create pipe for interprocess signalling.
Grandparent: get process ID.
Grandparent PID=46783
Grandparent: create process tree.
Parent PID=46784
Grandparent: exiting.
Child PID=46785
Parent: exiting.
Child: get parent PID after parent exits.
Child: Parent PID=1
Child: close write end of pipe.
Child: waiting for grandparent to close read end of pipe.
Child: Grandarent has closed read end of pipe
Child: get parent PID after grandparent executes wait().
Child: Parent PID=1
Child: exiting.
```

