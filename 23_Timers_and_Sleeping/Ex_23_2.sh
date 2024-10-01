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

for pgm in t_nanosleep Ex_23_2 t_clock_nanosleep
do
    if [[ ! -x "${pgm}" ]]
    then
        printf "Program %s not found\n" "${pgm}"
        continue
    fi

    log_dir="logs/Ex_23_2/${pgm}"
    [[ ! -d "${log_dir}" ]] && mkdir -p "${log_dir}"

    printf "Using %s...\n" "${pgm}"
    ./${pgm} 60 0 >${log_dir}/out.log 2>${log_dir}/err.log &
    job_pid=$!
    
    # --------------------------------------------------------------------------
    # The Linux command, kill, is used instead of the shell builtin in order to
    # slow down the loop enough to allow the background job to complete.
    # --------------------------------------------------------------------------

    while true
    do
        /usr/bin/kill -INT ${job_pid} 2>/dev/null || break
    done
    wait
    tail -n 3 ${log_dir}/out.log
done

exit 0
