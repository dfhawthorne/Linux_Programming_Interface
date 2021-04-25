# ------------------------------------------------------------------------------
# Make file for Chapter 15: File Attributes
# ------------------------------------------------------------------------------

.PHONY : all
all : t_stat my_access

t_stat: t_stat.c
	gcc -o t_stat t_stat.c file_perms.c

my_access: my_access.c
	chown test_user my_access
	chmod u+s my_access

%: %.c
	gcc -o $@ $^ 
