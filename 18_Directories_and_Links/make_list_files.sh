#!/bin/bash
include_dirs="-I ../03_System_Programming_Concepts/"
programs="list_files.c ../03_System_Programming_Concepts/error_functions.c"
gcc -o list_files ${include_dirs} ${programs}
