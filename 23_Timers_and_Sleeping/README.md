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
Remaining:  0.000715167
Slept for: 62.077613 secs
Sleep complete
Using Ex_23_2...
Remaining: 9437184.000000012
Slept for: 60.000036 secs
Sleep complete
Using t_clock_nanosleep...
Interrupted... Slept: 62.072626 secs... Remaining: 0.002018094... Restarting
Slept: 62.074727 secs
Sleep complete
```

There is no meaningful difference between the two (2) versions: t_nanosleep; and t_clock_nanosleep. Both of them overran the set interval by about two (2) seconds.

My solution was close to the set interval.

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
