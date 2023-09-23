# Chapter 22: Signals: Advanced Features

## Table of Contents

* [Exercise 22-1](exercise-22-1)
* [Exercise 22-2](exercise-22-2)

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
```
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
```
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