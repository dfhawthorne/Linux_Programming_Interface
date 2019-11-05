# Chapter 6: Processes

## Exercise 6-1

The source code is downloaded from [mem_segments.c](http://man7.org/tlpi/code/online/dist/proc/mem_segments.c.html).

To do the exercise:
1. Use `make mem_segments` to compile and link the code.
1. Use `ls -l mem_segments` to see the size of the code.

The output should be similar to:
```
-rwxr-xr-x 1 douglas douglas 18848 Nov  4 11:22 mem_segments
```

The answer is given on p. 116:
> ...The main reason for placing global and static variables that are initialized into a separate segment from those that are unitialized is that, __when a program is stored on disk, it is not necessary to allocate space for unitialized data.__ Instead, the executable merely needs to record the location and size required for the uninitialized data segment, and this space is allocated by the praogram loader at run time.

## Exercise 6-2

My solution is in `ex6_2.c` which is a modified version of [longjmp.c](http://man7.org/tlpi/code/online/dist/proc/longjmp.c.html). The real solution can be found in [bad_longjmp.c](http://man7.org/tlpi/code/online/dist/proc/bad_longjmp.c.html).

To do the exercise:
1. Use `make ex6_2` to compile and link the code.
1. Use `./ex6_2` to execute the solution.

The output should be similar to:
```
Calling f1() after initial setjmp()
test_jmp(1) finished
We jumped back from f1()
test_jmp(1) finished
```

In effect, the body of the function is executed twice.

However, the solution (`./bad_longjmp`) provided does the following:
```
Entered setJump
Entered setJump2
Exiting setJump2
Exiting setJump
Entered doJump
Exiting setJump2
Entered setJump2
Exiting setJump2
Segmentation fault (core dumped)
```

## Exercise 6-3

My solution is in `ex6_3.c`.  The real solution is in [setenv.c](http://man7.org/tlpi/code/online/dist/proc/setenv.c.html).

1. Use `make ex6_3` to compile and link the code.
1. Use `./ex6_3` to test the solution.

The expected output is:
```
FRED=NURK
FRED=NURK
FRED=charlie
FRED=NULL
```

To run the real solution:
1. Use `make setenv` to compile and link the code.
1. Use `./setenv` to execute the code.

The output should be similar to:
```
***** Environment before unsetenv(TT)
TT=xxxxx
Total lines from printenv: 58
***** Environment after unsetenv(TT)
Total lines from printenv: 57
***** Environment after setenv() calls
xyz=one
xyz2=222
Total lines from printenv: 59
```
