# Chapter 21: Signals: Signal Handlers

## Table of Contents

* [Exercise 21-1](exercise-21-1)
  * [Use abort](use-abort)
  * [Use my_abort](use-my-abort)

## Exercise 21-1

### Use abort

To test solution using `abort()`, run:
```bash
make -s test_abort_1;cat test1.log
```
Sample output is:
```text
make: *** [Makefile:22: test_abort_1] Aborted (core dumped)
signal handler installed for SIGABRT.
before call to abort()
Signal caught
```

### Use my_abort

To test my implementation of `abort()`, run:
```bash
make -s test_abort_2;cat test2.log
```
Sample output is:
```text
make: *** [Makefile:22: test_abort_2] Aborted (core dumped)
signal handler installed for SIGABRT.
before call to my_abort()
before call to signal handler.
Signal caught
```