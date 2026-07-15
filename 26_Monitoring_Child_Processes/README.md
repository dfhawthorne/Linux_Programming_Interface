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

## Exercise 26-3

Run:

```bash
make test_3
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 child_status.c print_wait_status.c error_functions.c get_num.c -o child_status
./child_status 10
Child started with PID = 36262
waitpid() returned: PID=36262; status=0x0000 (0,0)
child killed by signal 10 (User defined signal 1)

Additional fields from siginfo_t
        Signal number=17
        Signal code=1
        Sending process ID=36262
        Real user ID of sending process=1000
        Memory location which caused fault=   0x3e800008da6
```
