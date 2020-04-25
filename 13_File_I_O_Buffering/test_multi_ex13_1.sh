#!/bin/bash
# ------------------------------------------------------------------------------
# Runs a series of tests for Exercise 13-1 multiple times and save results.
# ------------------------------------------------------------------------------

for size in $(seq 1024 1024 16384)
do
    for iter in $(seq 1 1 10)
    do
        ./test_ex13_1.sh ${size} >>ex13_1_$(date -I).log
    done
done
