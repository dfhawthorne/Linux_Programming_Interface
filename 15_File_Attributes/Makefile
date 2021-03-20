# ------------------------------------------------------------------------------
# Make file for Chapter 15: File Attributes
# ------------------------------------------------------------------------------

.PHONY : all
all : t_stat my_access

t_stat: t_stat.c
	gcc -o t_stat t_stat.c file_perms.c

%: %.c
	gcc -o $@ $^ 
