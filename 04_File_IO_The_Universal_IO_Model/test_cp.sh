#!/bin/bash
# ------------------------------------------------------------------------------
# Tests copying of sparse files
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Copy a non-sparse file first
# ------------------------------------------------------------------------------

rm -f test.lst test.dat

pre_usage=$(df --block-size=4096 --output=used . | tail -n 1)
./cp cp.c test.lst
post_usage=$(df --block-size=4096 --output=used . | tail -n 1)
no_sparse_usage=$((( post_usage - pre_usage )))

# ------------------------------------------------------------------------------
# Create a sparse file
# ------------------------------------------------------------------------------

rm -f test.lst test.dat

pre_usage=$(df --block-size=4096 --output=used . | tail -n 1)
dd if=/dev/zero of=test.dat count=6 bs=4096 >/dev/null 2>&1
post_usage=$(df --block-size=4096 --output=used . | tail -n 1)
sparse_usage=$((( post_usage - pre_usage )))

pre_usage=$(df --block-size=4096 --output=used . | tail -n 1)
./cp test.dat test.lst
post_usage=$(df --block-size=4096 --output=used . | tail -n 1)
sparse_copy_usage=$((( post_usage - pre_usage )))

# ------------------------------------------------------------------------------
# Print report
# ------------------------------------------------------------------------------

rm -f test.lst test.dat

printf "Non-sparse file used     %3d x 4K blocks\n" ${no_sparse_usage}
printf "Sparse file used         %3d x 4K blocks\n" ${sparse_usage}
printf "Copy of sparse file used %3d x 4K blocks\n" ${sparse_copy_usage}
