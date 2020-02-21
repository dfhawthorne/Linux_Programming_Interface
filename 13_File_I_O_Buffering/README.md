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
