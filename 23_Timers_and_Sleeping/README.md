# Chapter 23: Timers and Sleeping

## Exercise 23-1

To run my solution, type:

```bash
make test_1
```

The sample output is:

```text
Alarm signal received!
Alarm succeeded
```

## Exercise 23-2

The published solution to [Exercise 23-2](https://man7.org/tlpi/code/online/dist/timers/t_clock_nanosleep.c.html) uses `clock_nanosleep()` instead of `nanosleep()`.

To run my solution along with the published solution, type:

```bash
make test_2
```

The sample output is:

```text
./Ex_23_2.sh
Using t_nanosleep...
Remaining:  0.000346101
Slept for: 61.986514 secs
Sleep complete
Using Ex_23_2...
Remaining:  0.000576292
Slept for: 61.888986 secs
Sleep complete
Using t_clock_nanosleep...
Interrupted... Slept: 62.007887 secs... Remaining: 0.000790663... Restarting
Slept: 62.008789 secs
Sleep complete
```

There is no meaningful difference between the three (3) versions. All of them overran the set interval by about two (2) seconds.

## Exercise 23-3

To run the test, use the following command:

```bash
make test_3
```

The sample output is:

```text
gcc -o Ex_23_3 Ex_23_3.c 
./Ex_23_3
Signal 14 caught
Signal caught
./Ex_23_3 EVP
Signal 14 caught
Signal caught
```
