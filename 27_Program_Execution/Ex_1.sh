#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Exercise 27-1:
#
# Use the program in listing 27-3 to execute xyz
# ------------------------------------------------------------------------------

PATH="/usr/local/bin:/usr/bin:/bin:./dir1:./dir2"

echo "${PATH}"
ls -l dir[12]
./t_execlp xyz
