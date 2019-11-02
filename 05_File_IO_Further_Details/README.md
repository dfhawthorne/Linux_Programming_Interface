# Chapter 5: File I/O: Further Details

## Exercise 5-1

My solution is in `large_file.c`.

To build and test this solution:
1. Use `make large_file` to compile and link the code.
1. Use `./large_file test.dat 10111222333` to test this code.
1. Use `stat test.dat` to verify the size

The expected response is similar to:
```
  File: test.dat
  Size: 10111222337	Blocks: 8          IO Block: 4096   regular file
Device: fd01h/64769d	Inode: 51643046    Links: 1
Access: (0600/-rw-------)  Uid: ( 1000/ douglas)   Gid: ( 1000/ douglas)
```

## Exercise 5-2

My solution is in `ex5_2.c`.

To build and test this solution:
1. Use `make ex5_2` to compile and link this code.
1. Use `./test_ex5_2.sh` to test this code.

The expected output is:
```
Contents of test file before running exercise 5-2
Line #1
Line #2
Now positioned at 0
Now positioned at 38

Contents of test file after running exercise 5-2
Line #1
Line #2
Exercise 5-2 completed
```

## Exercise 5-3

My solution is in `atomic_append.c`.

To build and test this solution:
1. Use `make atomic_append` to compile and link this code.
1. Use `./test_atomic_append.sh` to test this code.

The expected output is:
```
Size of test file using O_APPEND
test.dat               1997532
Size of test file without using O_APPEND
test.lst               1000115
```

__Why is the size of `test.dat` not 2000000?__
