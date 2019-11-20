# Chapter 9: Process Credentials

## Exercise 9-1

My solution is in `ex9_1.c`.

To compile and link this solution, use `sudo make ex9_1`.

### Exercise 9-1 (a)

To answer this part of the exercise, use `./ex9_1 a`. The output should be similar to:
```
Initial UIDs:
Uid:	1000	0	0	0
UIDs after setuid(1001):
Uid:	1001	1001	1001	1001
```

### Exercise 9-1 (b)

To answer this part of the exercise, use `./ex9_1 b`. The output should be similar to:
```
Initial UIDs:
Uid:	1000	0	0	0
UIDs after setreuid(-1, 1001):
Uid:	1000	1001	1001	1001
```

### Exercise 9-1 (c)

To answer this part of the exercise, use `./ex9_1 c`. The output should be similar to:
```
Initial UIDs:
Uid:	1000	0	0	0
UIDs after seteuid(1001):
Uid:	1000	1001	0	1001
```

### Exercise 9-1 (d)

To answer this part of the exercise, use `./ex9_1 d`. The output should be similar to:
```
Initial UIDs:
Uid:	1000	0	0	0
UIDs after setfsuid(1001):
Uid:	1000	0	0	1001
```

### Exercise 9-1 (e)

To answer this part of the exercise, use `./ex9_1 e`. The output should be similar to:
```
Initial UIDs:
Uid:	1000	0	0	0
UIDs after setresuid(-1, 1001, 1002):
Uid:	1000	1001	1002	1001
```

## Exercise 9-2

The answer is _NO_. The reason is given on p.168:
> A process whose effective user ID is 0 (the user ID of _root_) has all of the privileges of the superuser. Such a process is referred to as a _privileged process_. ...

## Exercise 9-3

My solution is in `my_initgroups.c`.

To run this solution:
1. Use `make my_initgroups` to compile and link this code.
1. Use `sudo ./my_initgroups` to execute this solution.

On successful execution, there is __no__ output.

## Exercise 9-4

My solution is in `ex9_4.c`.

To compile and link this solution, use `sudo make ex9_4`.

### Exercise 9-4 (a)

To answer this part of the exercise, use `./ex9_4 a`. The output should be similar to:
```
Initial UIDs:
Uid:    1000    1002    1002    1002
UIDs after seteuid(1000):
Uid:    1000    1000    1002    1000
UIDs after seteuid(1002):
Uid:    1000    1002    1002    1002
```

### Exercise 9-1 (b)

To answer this part of the exercise, use `./ex9_4 b`. The output should be similar to:
```
Initial UIDs:
Uid:    1000    1002    1002    1002
UIDs after setresuid(-1, 1000, 1000):
Uid:    1000    1000    1000    1000
```

## Exercise 9-5
