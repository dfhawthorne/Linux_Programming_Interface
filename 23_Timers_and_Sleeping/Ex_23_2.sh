#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Exercise 23-2:
#
# Try running the program (t_nanosleep.c) in the background with a 60-second
# sleep interval, while sending as many SIGINT signals as possible to the back-
# ground process.
#
# You should observe that the program sleeps rather longer than expected. Re-
# place the use of nanosleep() with the use of clock_gettime() (use a 
# CLOCK_REALTIME clock) and clock_nanosleep() with the TIMER_ABSTIME flag.
#
# Repeat the test with the modified program and explain the difference
# ------------------------------------------------------------------------------

pgm_dir=$(dirname $0)
pushd "${pgm_dir}" >/dev/null

for pgm in t_nanosleep Ex_23_2
do
    if [[ -x t_nanosleep ]]
    then
        printf "Using %s...\n" "${pgm}"
        "./${pgm}" 60 0 &
        pid=$!
        while true
        do
            kill -INT ${pid} 2>/dev/null || break
        done
    else
        printf "Program %s not found\n" "${pgm}"
    fi
done
