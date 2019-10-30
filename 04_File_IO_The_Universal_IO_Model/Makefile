# ------------------------------------------------------------------------------
# Make file for Chapter 04: File I/O: The Universal I/O Model
# ------------------------------------------------------------------------------

.PHONY : all
all : tee cp seek_io

cp : cp.c
	gcc -o cp cp.c

tee : tee.c
	gcc -o tee tee.c

seek_io : seek_io.c
	gcc -o seek_io -I ../03_System_Programming_Concepts/ seek_io.c ../03_System_Programming_Concepts/error_functions.c ../03_System_Programming_Concepts/get_num.c
