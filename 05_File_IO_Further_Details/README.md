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
test.dat               2000000
Size of test file without using O_APPEND
test.lst               1000089
```

__The size of `test.lst` can vary__

## Exercise 5-4

My solution is in `ex5_4.c`.

To build and test this solution:
1. Use `make ex5_4` to compile and link this code.
1. Use `./ex5_4` to test this code.

The expected output should be similar to:
```
Original file descriptors
FD (3) points to inode #51643046
FD (4) points to inode #51643592
Duplicate file descriptors
FD (5) points to inode #51643046
FD (6) points to inode #51643592
Reallocate file descriptor
FD (5) points to inode #51643592
Failed to duplicate fd #20: Bad file descriptor
Failed to duplicate fd #20 as #19: Bad file descriptor
```

## Exercise 5-5

My solution is in `ex5_5.c`.

To build and test this solution:
1. Use `make ex5_5` to compile and link this code.
1. Use `./ex5_5` to test this code.

The expected output should be similar to:
```
Original file descriptor
FD (3) points to inode #51643046, has a status of 0X8000, and an offset of 0
Duplicate file descriptor
FD (4) points to inode #51643046, has a status of 0X8000, and an offset of 0
Both file descriptors after seeking to EOF
FD (3) points to inode #51643046, has a status of 0X8000, and an offset of 2000000
FD (4) points to inode #51643046, has a status of 0X8000, and an offset of 2000000
```

## Exercise 5-6

My solution is in `ex5_6.c`.

To build and test this solution:
1. Use `make ex5_6` to compile and link this code.
1. Use `./ex5_6` to test this code.

The expected output should be:
```
Now doing 1 writes
The contents, after 1 writes, are:
Hello,
Now doing 2 writes
The contents, after 2 writes, are:
Hello,world 
Now doing 3 writes
The contents, after 3 writes, are:
HELLO,world 
Now doing 4 writes
The contents, after 4 writes, are:
Giddayworld 
```

## Exercise 5-7

My solution is in `ex5_7.c`. This solution uses code from [t_read.c](http://man7.org/tlpi/code/online/dist/fileio/t_readv.c.html).

To build and test this solution:
1. Use `make ex5_7` to compile and link this code.
1. Use `./ex5_7` to test this code.

The expected output should be:
```
total bytes requested: 248; bytes written: 248
Test write succeeded
total bytes requested: 248; bytes read: 248
Test read succeeded
```
