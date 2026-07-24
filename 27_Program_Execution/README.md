# 27: Program Execution

## Exerecise 27-1

Run:

```bash
make test_1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 t_execlp.c error_functions.c get_num.c -o t_execlp
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 dir1/xyz.c -o dir1/xyz
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 dir2/xyz.c -o dir2/xyz
./Ex_1.sh
/usr/local/bin:/usr/bin:/bin:./dir1:./dir2
dir1:
total 20
-rwxrwxr-x 1 douglas douglas 15968 Jul 21 08:36 xyz
-rw-rw-r-- 1 douglas douglas   124 Jul 21 08:31 xyz.c

dir2:
total 20
-rwxrwxr-x 1 douglas douglas 15968 Jul 21 08:36 xyz
-rw-rw-r-- 1 douglas douglas   124 Jul 21 08:31 xyz.c
Progam xyz executed from dir1/
```

## Exercise 27-2

Run:

```bash
make test_2
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_2.c -o Ex_2
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_2_child.c -o Ex_2_child
./Ex_2.sh 
Parent spawned PID=18666
Ex_2_child: Arguments passed
        1: "string"
        2: "2000"
        3: "c"
Environment variable ("USER") has value ("douglas")
```

## Exercise 27-3

Run:

```bash
make test_3
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_3.c -o Ex_3
./Ex_3_test.sh 
Parent spawned PID=19500
     1  #!/bin/cat -n
     2  Hello world
```

## Exercise 27-4

Run:

```bash
make test_4 VERBOSE=1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_4.c -o Ex_4
./Ex_4 -v
Parent initialises semaphore
Parent spawns child
Parent continues
Child spawns grandchild
Child makes grandchild an orphan
Grandchild does real work
Parent continues
UID          PID    PPID  C STIME TTY          TIME CMD
douglas   166139  166132  0 22:32 pts/0    00:00:00 ./Ex_4 -v
douglas   166141    3702  0 22:32 pts/0    00:00:00 ./Ex_4 -v
UID          PID    PPID  C STIME TTY          TIME CMD
douglas   166139  166132  0 22:32 pts/0    00:00:00 ./Ex_4 -v
douglas   166141    3702  0 22:32 pts/0    00:00:00 ./Ex_4 -v
```

To see what process with pid=3702 is, run:

```bash
ps -fp 3702
```

Sample output is:

```text
UID          PID    PPID  C STIME TTY          TIME CMD
douglas     3702       1  0 Jul23 ?        00:00:08 /usr/lib/systemd/systemd --user
```

Such code could be used to create daemons.
