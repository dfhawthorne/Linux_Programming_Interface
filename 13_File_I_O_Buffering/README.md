Chapter 13: File I/O Buffering
==============================

# Exercise 13-1

To build the solution, run `make ex13_1`.

The command options are:
1. __-b__ block_size - sets the size of the input buffer
1. __-s__ - enables the O_SYNC flag when opening the output file
1. __-v__ - shows more detailed output
1. input file name
1. output file name

To generate a log file of multiple runs, do
```bash
./test_ex13_1.sh >>test_run_2020_02_20.log
```

Output file has the following fields:
1. Block Size
2. Synchronisation flag set?
3. Number of bytes read
4. Wall clock time in seconds
5. User time in seconds
6. System time in seconds

## Part (a)

```bash
$ ./ex13_1 -b 512  /tmp/ex13_1.in /tmp/ex13_1.out 
512     -       67108864        0.28    0.07    0.21
$ ./ex13_1 -b 65536 /tmp/ex13_1.in /tmp/ex13_1.out 
65536   -       67108864        0.09    0.00    0.08
```

__Note__ The fourth column (Wall clock time) appears not to match the output from `time`.

Increasing the internal buffer size from 512 to 65536 (64K) reduced the overall time (user + sys) from 0.21s to 0.08s (a 62% reduction).

## Part (b)

```bash
$ ./ex13_1 -b 65536 -s /tmp/ex13_1.in /tmp/ex13_1.out 
65536   s       67108864        0.41    0.00    0.40
$ ./ex13_1 -b 512 -s /tmp/ex13_1.in /tmp/ex13_1.out 
512     s       67108864        27.10   1.51    25.59
```

__Note__ The actual elapsed time for the 512 byte buffer case was several hours (not 27 seconds).

Adding `O_SYNC` significantly increasing the run time (but the effect is much less with larger internal buffers).

## Part (c)

Not done yet.
