# Chapter 22: Signals: Advanced Features

## Table of Contents

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
./Ex_22_1 2023-08-27 00:58:20: Signal handler added - idling started
./Ex_22_1 2023-08-27 00:58:25: Signal caught
./Ex_22_1 2023-08-27 01:00:01: Idling ended
```

To experiment with masking, run:
```bash
./Ex_22_1.sh --mask
```

Sample output is:
```
./Ex_22_1 2023-08-27 00:13:32: Signal handler added - idling started
./Ex_22_1 2023-08-27 00:15:13: Idling ended
./Ex_22_1 2023-08-27 00:15:13: Signal caught
```

### Log Files

* `logs/test1/Ex_22_1_main.log` contains the above output.
* `logs/test1/Ex_22_1_stdout.log` contains any output sent to STDOUT from Ex_22_1.
* `logs/test1/Ex_22_1.log` contains any error messages, and verbose messages if the `--verbose` option is chosen.