# Chapter 12: System and Process Information

## Exercise 12-1

My solution is in `ex12_1.c`.

1. To compile and link this solution, use `make ex12_1`.
1. To execute this solution, use `./ex12_1`

The expected output is similar to:
```
    1811 systemd
    1812 (sd-pam)
    1825 gnome-keyring-d
    1829 gdm-x-session
    1831 Xorg
```

## Exercise 12-2

My solution is in `ex12_2.c`.

1. To compile and link this solution, use `make ex12_2`.
1. To execute this solution, use `./ex12_2`

The expected output is similar to:
```
** Unknown **
  ssh-agent
  cygtermd
    bash
      ex12_2
```

## Exercise 12-3

My solution is in `ex12_3.c`.

1. To compile and link this solution, use `make ex12_3`.
1. Run `tail -f Makefile` from another terminal
1. To execute this solution, use `./ex12_3 Makefile` in a different to that in step 2.

The expected output is similar to:
```
PID: 9665, FD: 3

```
