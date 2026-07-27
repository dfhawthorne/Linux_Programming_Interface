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
make test_4
./Ex_4.sh 
UID          PID    PPID  C STIME TTY          TIME CMD
douglas    15310   15308  0 17:37 pts/0    00:00:00 ./Ex_4
douglas    15312    2977  0 17:37 pts/0    00:00:00 ./Ex_4
Program has terminated. But grandchild is still running
UID          PID    PPID  C STIME TTY          TIME CMD
douglas    15312    2977  0 17:37 pts/0    00:00:00 ./Ex_4
Grandchild is now owned by the following process
UID          PID    PPID  C STIME TTY          TIME CMD
douglas     2977       1  0 15:55 ?        00:00:01 /usr/lib/systemd/systemd --user
Grandchild is now killed
```

Such code could be used to create daemons that are owned by `systemd`.

## Exercise 27-5

Run:

```bash
make test_5
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_5.c -o Ex_5
./Ex_5.sh
Run program with standard buffering on stdout
Run program with no buffering on stdout
Hello World
```

In the first case, all output is lost as `exec()` replace the program text and
data. The normal behaviour for output to stdout is store the output in the
program data area. This is validated when buffering is disabled for stdout.
