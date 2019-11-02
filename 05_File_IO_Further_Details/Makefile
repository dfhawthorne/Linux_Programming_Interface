# ------------------------------------------------------------------------------
# Make file for Chapter 04: File I/O: The Universal I/O Model
# ------------------------------------------------------------------------------

.PHONY : all
all : large_file ex5_2 atomic_append ex5_4

large_file : large_file.c
	gcc -o large_file -I ../03_System_Programming_Concepts/ large_file.c ../03_System_Programming_Concepts/error_functions.c 

ex5_2: ex5_2.c
	gcc -o ex5_2 ex5_2.c

atomic_append: atomic_append.c
	gcc -o atomic_append atomic_append.c

ex5_4: ex5_4.c
	gcc -o ex5_4 ex5_4.c

