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

I need to submit an erratum.

To run my solution, type:

```bash
make test_2
```

The sample output is:

```text
Using t_nanosleep...
Remaining:  0.001685607
Slept for: 62.019164 secs
Sleep complete
Using Ex_23_2...
Remaining:  0.000949108
Slept for: 62.014362 secs
Sleep complete
```

There is no meaningful difference between the two (2) versions. Both of them overran the set interval by about two (2) seconds.
