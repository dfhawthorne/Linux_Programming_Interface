#!/bin/bash
# ------------------------------------------------------------------------------
# Tests copying of sparse files
# ------------------------------------------------------------------------------

io_blk_size=$(stat --file-system --format="%S" .)

# ------------------------------------------------------------------------------
# Copy a non-sparse file first
# ------------------------------------------------------------------------------

rm -f test.lst test.dat

pre_free_blks=$(stat --file-system --format="%a" .)
./cp cp.c test.lst
post_free_blks=$(stat --file-system --format="%a" .)
no_sparse_usage=$((( pre_free_blks - post_free_blks )))

# ------------------------------------------------------------------------------
# Create a sparse file
# ------------------------------------------------------------------------------

rm -f test.lst test.dat

pre_free_blks=$(stat --file-system --format="%a" .)
touch test.dat
./seek_io test.dat s1000000 wabc
post_free_blks=$(stat --file-system --format="%a" .)
sparse_usage=$((( pre_free_blks - post_free_blks )))

pre_free_blks=$(stat --file-system --format="%a" .)
./cp test.dat test.lst
post_free_blks=$(stat --file-system --format="%a" .)
sparse_copy_usage=$((( pre_free_blks - post_free_blks )))

# ------------------------------------------------------------------------------
# Print report
# ------------------------------------------------------------------------------

rm -f test.lst test.dat

printf "Non-sparse file used     %3d x %d blocks\n" ${no_sparse_usage}   ${io_blk_size}
printf "Sparse file used         %3d x %d blocks\n" ${sparse_usage}      ${io_blk_size}
printf "Copy of sparse file used %3d x %d blocks\n" ${sparse_copy_usage} ${io_blk_size}
