#!/bin/bash
include_dirs="-I ../03_System_Programming_Concepts/"
programs="realpath.c ../03_System_Programming_Concepts/error_functions.c"
gcc -o realpath ${include_dirs} ${programs} -lacl
