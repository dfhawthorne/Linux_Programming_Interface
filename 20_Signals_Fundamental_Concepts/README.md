# Chapter 20: Signals: Fundamental Concepts

## Table of Contents

* [Exercise 20-1](#exercise-20-1)
  * [Exercise 20-1 Without Sleeping](#exercise-20-1-without-sleeping)
  * [Exercise 20-1 With Sleeping](#exercise-20-1-with-sleeping)
* [Exercise 20-2](#exercise-20-2)
* [Exercise 20-3](#exercise-20-3)
  * [Exercise 20-3 with SA_NODEFER](#test-sa_nodefer)
  * [Exercise 20-3 with SA_RESETHAND](#test-sa_resethand)
* [Exercise 20-4](#exercise-20-4)

## Exercise 20-1

### Exercise 20-1 Without Sleeping

To run the solution without sleeping, use:
```bash
make test_1_n
```

The `logs/test1/receiver.log` file shows:
```text
sig_receiver: PID=21035
sig_receiver: signal  10 caught 499865 time(s)
```

The `logs/test1/sender.log` file shows:
```text
sig_sender: 1000000 signals (signal=10) are to be sent to process (PID=21035) with a last signal of 2.
sig_sender: Completed.
```

To get verbose output, use:
```bash
make test_1_n_v
```

### Exercise 20-1 With Sleeping

To run the solution with sleeping, use:
```bash
make test_1_s
```

The `logs/test1/receiver.log` file shows:
```text
sig_receiver: PID=21713
sig_receiver: sleeping for 5 seconds.
sig_receiver: pending signals are:
		 10 (User defined signal 1)
sig_receiver: signal  10 caught      1 time(s)
```

The `logs/test1/sender.log` file shows:
```text
sig_sender: 1000000 signals (signal=10) are to be sent to process (PID=21713) with a last signal of 2.
sig_sender: Completed.
```

To get verbose output, use:
```bash
make test_1_s_v
```

## Exercise 20-2

To run the solution with sleeping, use:
```bash
make test_2
```

The `logs/test2/sender.log` file shows:
```text
sig_sender: 1000000 signals (signal=10) are to be sent to process (PID=13960) with a last signal of 2.
sig_sender: Completed.
```

The `logs/test2/receiver.log` file shows:
```text
sig_receiver: PID=13960
sig_receiver: signal (10) will be ignored.
sig_receiver: sleeping for 5 seconds.
sig_receiver: pending signals are:
		 10 (User defined signal 1)
sig_receiver: No signals were caught.
```

To get verbose output, use:
```bash
make test_2_v
```

## Exercise 20-3

### Test SA_NODEFER

To run tests with the SA_NODEFER set:
```bash
make test_3_n
```
Sample output (from `test/logs/test3/sig_receiver.log`) is:
```text
sig_receiver: PID=37910
sig_receiver: signals are handled with SA_NODEFER.
sig_receiver: No signals will be ignored.
sig_receiver: signal  10 caught      1 time(s)
```

### Test SA_RESETHAND

To run tests with the SA_RESETHAND set:
```bash
make test_3_r
```
Sample output (from `test/logs/test3/sig_receiver.log`) is:
```text
sig_receiver: PID=38239
sig_receiver: signals are handled with SA_RESETHAND.
sig_receiver: No signals will be ignored.
sig_receiver: signal  10 caught      1 time(s)
```

## Exercise 20-4

To run the solution, type:
```bash
make test_4
```
Sample output is:
```text
make test_4
gcc -o sig_interrupt sig_interrupt.c 
./run_test_4.sh 
sig_interrupt: Enable signal handler for SIGUSR1
sig_interrupt: Send SIGUSR1 to self
sig_interrupt: signal was received.
sig_interrupt: setting of signal handler (signal_handler) through siginterrupt(10,0)
sig_interrupt: Send SIGUSR1 to self
sig_interrupt: signal was received.
sig_interrupt: setting of signal handler (signal_handler) through siginterrupt(10,1)
sig_interrupt: Send SIGUSR1 to self
sig_interrupt: signal was received.
sig_interrupt: Exit.
```