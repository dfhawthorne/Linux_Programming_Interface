# ------------------------------------------------------------------------------
# Make file for Chapter 06: Processes
# ------------------------------------------------------------------------------

.PHONY : all
all : mem_segments longjmp ex6_2 bad_longjmp

mem_segments: mem_segments.c
	gcc -o mem_segments -I ../03_System_Programming_Concepts/ mem_segments.c ../03_System_Programming_Concepts/error_functions.c

longjmp: longjmp.c
	gcc -o longjmp -I ../03_System_Programming_Concepts/ longjmp.c ../03_System_Programming_Concepts/error_functions.c

ex6_2: ex6_2.c
	gcc -o ex6_2 ex6_2.c

bad_longjmp: bad_longjmp.c
	gcc -o bad_longjmp -I ../03_System_Programming_Concepts/ bad_longjmp.c ../03_System_Programming_Concepts/error_functions.c
