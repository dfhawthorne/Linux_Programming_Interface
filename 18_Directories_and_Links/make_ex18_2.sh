#!/bin/bash
include_dirs="-I ../03_System_Programming_Concepts/"
programs="ex18_2.c ../03_System_Programming_Concepts/error_functions.c"
gcc -o ex18_2 ${include_dirs} ${programs}
