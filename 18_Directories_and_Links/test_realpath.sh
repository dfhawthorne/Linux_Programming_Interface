#!/bin/bash
# ---------------------------------------------------------------------
# Test cases for my version of realpath
# ---------------------------------------------------------------------
./realpath /
./realpath //
./realpath /.
./realpath /./
./realpath /etc/..
./realpath /etc/../
./realpath ./test1.dat
./realpath ./test.dat
./realpath //etc
./realpath //etc/
