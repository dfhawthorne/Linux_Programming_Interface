#!/bin/bash
# ------------------------------------------------------------------------------------------
# Tests exercise 5-2
# ------------------------------------------------------------------------------------------

cat >test.dat <<EOF
Line #1
Line #2
EOF

printf "Contents of test file before running exercise 5-2\n"
cat test.dat
printf "\nContents of test file after running exercise 5-2\n"
cat test.dat
