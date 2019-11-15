# Chapter 9: Process Credentials

## Exercise 9-1

## Exercise 9-2

The answer is _NO_. The reason is given on p.168:
> A process whose effective user ID is 0 (the user ID of _root_) has all of the privileges of the superuser. Such a process is referred to as a _privileged process_. ...

## Exercise 9-3

My solution is in `my_initgroups.c`.

To run this solution:
1. Use `make my_initgroups` to compile and link this code.
1. Use `sudo ./my_initgroups` to execute this solution.

On successful execution, there is __no__ output.

## Exercise 9-4

## Exercise 9-5
