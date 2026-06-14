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

## Exercise 23-4

To run the test, use the following command:

```bash
make test_4
```

The sample output is:

```text
gcc -o ptmr_sigev_thread ptmr_sigev_thread.c error_functions.c get_num.c itimerspec_from_str.c curr_time.c
From LPI (p.503),
Run './ptmr_sigev_thread 2:5'
Wait until timer expires a second time.
Press <ctrl-z> to suspend the process
Wait for a few seconds, then run 'fg'
douglas@coogee:23_Timers_and_Sleeping$ ./ptmr_sigev_thread 2:5
Timer ID: -9223319418302678448 (2:5)
[04:47:55] Thread notify
    timer ID=-9223319418302678448
    timer_getoverrun()=0
main(): expireCnt = 1
[04:48:00] Thread notify
    timer ID=-9223319418302678448
    timer_getoverrun()=0
main(): expireCnt = 2
^Z
[1]+  Stopped                 ./ptmr_sigev_thread 2:5
douglas@coogee:23_Timers_and_Sleeping$ fg
./ptmr_sigev_thread 2:5
[04:48:51] Thread notify
    timer ID=-9223319418302678448
    timer_getoverrun()=9
main(): expireCnt = 12
[04:48:55] Thread notify
    timer ID=-9223319418302678448
    timer_getoverrun()=0
main(): expireCnt = 13
[04:49:00] Thread notify
    timer ID=-9223319418302678448
    timer_getoverrun()=0
main(): expireCnt = 14
[04:49:05] Thread notify
    timer ID=-9223319418302678448
    timer_getoverrun()=0
main(): expireCnt = 15
[04:49:10] Thread notify
    timer ID=-9223319418302678448
    timer_getoverrun()=0
main(): expireCnt = 16
^C
douglas@coogee:23_Timers_and_Sleeping$ 
```
