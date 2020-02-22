#!/bin/bash
# ------------------------------------------------------------------------------
# Runs a series of tests for Exercise 13-1.
# ------------------------------------------------------------------------------

if [ -z "$1" ]
then count=1024
else count=$1
fi

dd if=/dev/zero bs=65536 count=${count} of=/tmp/ex13_1.in

block_sizes="512 1024 2048 4096 8192 16384 32768 65536"

printf "\nRun timing tests without sync flag\n\n" >&2

for blksz in ${block_sizes}
do
    printf "For blocksize=%d:\n" ${blksz} >&2
    ./ex13_1 -b ${blksz} /tmp/ex13_1.in /tmp/ex13_1.out
done

printf "\nRun timing tests with sync flag\n\n" >&2

for blksz in ${block_sizes}
do
    printf "For blocksize=%d:\n" ${blksz} >&2
    ./ex13_1 -b ${blksz} -s /tmp/ex13_1.in /tmp/ex13_1.out
done
