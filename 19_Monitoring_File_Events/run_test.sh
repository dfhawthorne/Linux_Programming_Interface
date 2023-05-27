#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Tests the mon_dir_evt for Exercise 19-1
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Change the working directory to that of this script
# ------------------------------------------------------------------------------

base_dir=$(dirname $0)
pushd ${base_dir} &>/dev/null

# ------------------------------------------------------------------------------
# Create the initial directory structure
# ------------------------------------------------------------------------------

mkdir -p test_dir/test{a..e}/test{1..3} test_other/test6
touch test_dir/test{a..e}/test{1..3}/test.dat test_other/test7.dat

# ------------------------------------------------------------------------------
# Run the monitor program in the background with any passed parameters
# ------------------------------------------------------------------------------

./mon_dir_evt $* test_dir >test.log 2>test.err &
echo $! >test.pid

# ------------------------------------------------------------------------------
# Do some simple tests based on a random shuffling of commands read in from an
# external data file (cases.dat) with a random sleep time between zero (0) and
# nine (9) seconds.
# ------------------------------------------------------------------------------

while read cmd 
do 
    eval "${cmd}"
    sleep $((( ${RANDOM} % 10 )))
done < <(shuf cases.dat)

# ------------------------------------------------------------------------------
# Wait for 10 seconds before killing the monitor job, if it is still running
# ------------------------------------------------------------------------------

sleep 10
pgrep --pidfile test.pid &>/dev/null && \
    kill $(cat test.pid)

# ------------------------------------------------------------------------------
# Restore the previous working directory
# ------------------------------------------------------------------------------

popd &>/dev/null
