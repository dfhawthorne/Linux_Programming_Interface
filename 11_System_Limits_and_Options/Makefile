# ------------------------------------------------------------------------------
# Make file for Chapter 11: System Limits and Options
# ------------------------------------------------------------------------------

.PHONY : all
all : t_fpathconf  t_sysconf

t_fpathconf: t_fpathconf.c
	gcc -o t_fpathconf -I ../03_System_Programming_Concepts/ t_fpathconf.c ../03_System_Programming_Concepts/error_functions.c

t_sysconf: t_sysconf.c
	gcc -o t_sysconf -I ../03_System_Programming_Concepts/ t_sysconf.c ../03_System_Programming_Concepts/error_functions.c
