# ------------------------------------------------------------
# Make file for Chapter 18: Directories and Links
# ------------------------------------------------------------

.PHONY : all
all : getcwd ex18_2 list_files realpath

getcwd : getcwd.c
	gcc -o getcwd getcwd.c

ex18_2 : ex18_2.c 
	gcc -o ex18_2 -I ../03_System_Programming_Concepts/ ex18_2.c ../03_System_Programming_Concepts/error_functions.c

list_files : list_files.c
	gcc -o list_files -I ../03_System_Programming_Concepts/ list_files.c ../03_System_Programming_Concepts/error_functions.c

realpath : realpath.c
	gcc -o realpath -I ../03_System_Programming_Concepts/ realpath.c ../03_System_Programming_Concepts/error_functions.c

