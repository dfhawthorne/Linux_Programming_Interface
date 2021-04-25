#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Tests Exercise 15-6
# ------------------------------------------------------------------------------

[[ -a test_dir ]] && chmod u+rwx test_dir && rm -fR test_dir
[[ -a test_file ]] && rm -f test_file
[[ -a test_prog ]] && rm -f test_prog

mkdir test_dir
touch test_file
touch test_prog

chmod 0000 test_dir test_file
chmod o+x test_prog

printf "Before chmod a+rX...\n"
ls -ld test_dir test_file test_prog

./chmod_arX test_dir
./chmod_arX test_file
./chmod_arX test_prog

printf "After chmod a+rX...\n"
ls -ld test_dir test_file test_prog

