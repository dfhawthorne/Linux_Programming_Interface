# Chapter 8: Users and Groups

## Exercise 8-1

The answer is given on p.154:
> It is possible (but unusual) to have __more than one record in the password file with the same user ID__, thus permitting multiple login names for the same user ID. This allows multiple users to access the same resources (e.g., files) using different passwords. The different login names can be associated with different sets of group IDs.

## Exercise 8-2

My solution is in `my_getpwnam.c`.

To use this solution:
1. Use `make my_getpwnam` to compile and link the code.
1. Use `./my_getpwnam douglas` to execute this code.
