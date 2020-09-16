Chapter 14: File Systems
========================

# Exercise 14-1

To build the solution, run `make ex14_1`.

The command options are:
1. __-n__ number of files to create
1. __-s__ create files in sequential order
1. __-v__ - shows more detailed output
1. target directory

Statistics collected:
1. File system type
1. Create files in random (r) or sequential (s) order
1. Number of files created
1. Wall clock time in seconds
1. User time in seconds 
1. System time in seconds

Sample run with random order of creation:
```bash
time ./ex14_1 -n 1000 test
0x03E706FF      r       1000    2.859000        0.109000        2.750000
real    0m 5.12s
user    0m 0.25s
sys     0m 4.45s
```

Sample run with sequential order of creation:
```bash
time ./ex14_1 -sn 1000 test
0x03E706FF      s       1000    2.547000        0.078000        2.469000
real    0m 4.35s
user    0m 0.21s
sys     0m 3.96s
```
