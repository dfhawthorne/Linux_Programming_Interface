# Chapter 21: Signals: Signal Handlers

## Table of Contents

## Exercise 21-1

Run:
```bash
make
```

The output is:
```text
rm -f my_abort
gcc -o my_abort my_abort.c 
./run_test_abort.sh
./run_test_abort.sh: line 6: 35864 Aborted                 (core dumped) ./my_abort
```
A file, called `test.log`, is created. Sample output is:
```text
signal handler installed for SIGABRT.
before call to signal handler.
Signal caught
```