#!/bin/bash
# ------------------------------------------------------------------------------------------
# Tests exercise 5-3 (atomic_append)
# ------------------------------------------------------------------------------------------

rm -f test.dat test.lst

./atomic_append test.dat 1000000 & ./atomic_append test.dat 1000000
printf "Size of test file using O_APPEND\n"
printf "%-20s %9d\n" $(stat --format="%n %s" test.dat)

./atomic_append test.lst 1000000 x & ./atomic_append test.lst 1000000 x
printf "Size of test file without using O_APPEND\n"
printf "%-20s %9d\n" $(stat --format="%n %s" test.lst)



