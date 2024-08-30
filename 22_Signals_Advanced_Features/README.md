# Chapter 22: Signals: Advanced Features

## Table of Contents

* [Exercise 22-1](#exercise-22-1)
* [Exercise 22-2](#exercise-22-2)
* [Exercise 22-3](#exercise-22-3)
* [Exercise 22-4](#exercise-22-4)

## Exercise 22-1

To build the solution, run:

```bash
make Ex_22_1
```

To experiment without masking, run:

```bash
./Ex_22_1.sh
```

Sample output is:

```text
./Ex_22_1 2023-08-27 12:33:33: Signal handler added
./Ex_22_1 2023-08-27 12:33:33: Idling started
./Ex_22_1 2023-08-27 12:33:38: Signal caught
./Ex_22_1 2023-08-27 12:35:14: Idling ended
```

To experiment with masking, run:

```bash
./Ex_22_1.sh --mask
```

Sample output is:

```text
./Ex_22_1 2023-08-27 12:35:15: Signal blocked
./Ex_22_1 2023-08-27 12:35:15: Signal handler added
./Ex_22_1 2023-08-27 12:35:15: Idling started
./Ex_22_1 2023-08-27 12:36:56: Idling ended
./Ex_22_1 2023-08-27 12:36:56: Signal unblocked
./Ex_22_1 2023-08-27 12:36:56: Signal caught
```

### Log Files

* `logs/test1/Ex_22_1_main.log` contains the above output.
* `logs/test1/Ex_22_1_stdout.log` contains any output sent to STDOUT from Ex_22_1.
* `logs/test1/Ex_22_1.log` contains any error messages, and verbose messages if the `--verbose` option is chosen.

## Exercise 22-2

To build and test the solution, run:

```bash
make test_2
```

Sample output can be found in the file [`Ex_22_2_sample_solution`](Ex_22_2_sample_solution).

The answer to the question is that real time signals are presented first.

## Exercise 22-3

To build and test the solution, run:

```bash
make test_3
```

Sample output is:

```text
test using sigsuspend completed  1000000 3.98 1.14 2.83
test using sigwaitinfo completed 1000000 0.68 0.35 0.32
```

Using `sigwaitinfo()` results in an 83% reduction in run time.

## Exercise 22-4

To build and test the solution, run:

```bash
make test_4
```

Sample output is:

```text
gcc -o Ex_22_4 Ex_22_4.c
./Ex_22_4.sh  --log-dir=logs/Ex_22_4
```

The contents of the stdout log is:

```text
my_sigset passed
my_sighold passed
my_sigignore passed
my_sigrelse passed
my_sigpause passed
*** All tests done ***
```
