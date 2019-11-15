# ------------------------------------------------------------------------------
# Make file for Chapter 09: Process Credentials
# ------------------------------------------------------------------------------

.PHONY : all
all : my_initgroups ex9_1

ex9_1: ex9_1.c
	gcc -o ex9_1 ex9_1.c
	chown root ex9_1
	chmod u+s ex9_1
	
my_initgroups: my_initgroups.c
	gcc -o my_initgroups my_initgroups.c
