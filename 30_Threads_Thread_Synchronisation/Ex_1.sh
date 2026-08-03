#!/usr/bin/env bash
# --------------------------------------------------------------------------------------
# Test harnass for Exercise 30-1:
#
# Modify the program in Listing 30-1 (thread_incr.c) so that each loop in the thread's
# start function outputs the current value of glob and some identifier that uniquely
# identifies the thread. The unique identifier for the thread could be specified as an
# argument to the pthread_create() call used to create the thread. For this program,
# that would require changing the argument of the thread's start function to be a
# pointer to a structure containing the unique identifier and a loop limit value. Run
# the program, redirecting output to a file, and then inspect the file to see what
# happens to glob as the kernel scheduler alternates execution between the two threads.
# --------------------------------------------------------------------------------------

if [[ -z "${LD_LIBRARY_PATH}" ]]
    then export LD_LIBRARY_PATH="../Shared_Libraries"
    else export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:../Shared_Libraries"
fi

./thread_incr >Ex_1.lst

num_dups="$(cut -d: -f2 Ex_1.lst | sort | uniq -c | grep -vE '^ *1 ' | wc -l)"
printf "Num of duplicate values found=%d\n" ${num_dups}

