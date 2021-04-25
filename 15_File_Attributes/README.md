# Chapter 15: File Attributes

## Table of Contents

  * [Exercise 15-1](#exercise-15-1)
    * [Exercise 15-1 (a)](#exercise-15-1-a)
    * [Exercise 15-1 (b)](#exercise-15-1-b)
    * [Exercise 15-1 (c)](#exercise-15-1-c)
  * [Exercise 15-2](#exercise-15-2)
  * [Exercise 15-3](#exercise-15-3)
  * [Exercise 15-4](#exercise-15-4)
  * [Exercise 15-5](#exercise-15-5)
  * [Exercise 15-6](#exercise-15-6)


## Exercise 15-1

### Exercise 15-1 (a)

My answer is in [Ex15_1A.md](Ex15_1A.md)

### Exercise 15-1 (b)

My answer is in [Ex15_1B.md](Ex15_1B.md)

### Exercise 15-1 (c)

1. Run `make my_access`
1. Run `./ex15_1c.sh >results.md`
1. Results are in [Ex15_1C.md](Ex15_1C.md)

## Exercise 15-2

My answer is in [Ex15_2.md](Ex15_2.md)

## Exercise 15-3

1. Run `make t_stat`
1. Run `touch test`
1. Run `./t_stat test`

Sample output is:
```
File type:                regular file
Device containing i-node: major=253   minor=1
I-node number:            51773935
Mode:                     100400 (r--------)
Number of (hard) links:   1
Ownership:                UID=1000   GID=1000
File size:                0 bytes
Optimal I/O block size:   4096 bytes
512B blocks allocated:    0
Last file access:         Thu Apr  1 08:26:36.752583228 2021
Last file modification:   Sun Mar 21 21:42:26.305140934 2021
Last status change:       Thu Apr  1 21:23:46.819378661 2021
```

## Exercise 15-4

1. Run `make my_access`
1. Run `sudo chown test_user my_access`
1. Run `sudo chmod u+sx my_access`
1. Run `touch test`
1. Run `chmod 400 test`
1. Run `./my_access -f test`
1. Run `./my_access -r test`
1. Run `./my_access -w test`

Sample output is:
```
$ ./my_access -f test
Access for uid to 'test' using mask 0 is 0 (Success)
seteuid(32767) failed: Operation not permitted
$ ./my_access -r test
Access for uid to 'test' using mask 4 is 0 (Success)
seteuid(32766) failed: Operation not permitted
$ ./my_access -w test
Access for uid to 'test' using mask 2 is -1 (Permission denied)
seteuid(32764) failed: Operation not permitted
```

## Exercise 15-5

My answer is in [EX_15_5.md](EX_15_5.md)

## Exercise 15-6

1. Run `make chmod_arX`
1. Run `./chmod_arX.sh`

Sample output is:
```
$ ./chmod_arX.sh
Before chmod a+rX...
d--------- 2 douglas douglas 4096 Apr 25 19:18 test_dir
---------- 1 douglas douglas    0 Apr 25 19:18 test_file
-rw-rw-r-x 1 douglas douglas    0 Apr 25 19:18 test_prog
After chmod a+rX...
dr-xr-xr-x 2 douglas douglas 4096 Apr 25 19:18 test_dir
-r--r--r-- 1 douglas douglas    0 Apr 25 19:18 test_file
-rwxrwxr-x 1 douglas douglas    0 Apr 25 19:18 test_prog
```
