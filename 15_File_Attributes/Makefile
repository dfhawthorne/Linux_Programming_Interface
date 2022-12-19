# ------------------------------------------------------------------------------
# Make file for Chapter 15: File Attributes
# ------------------------------------------------------------------------------

.PHONY : all
all : t_stat my_access chmod_arX my_chattr run_test_chattr

t_stat: t_stat.c
	gcc -o t_stat t_stat.c file_perms.c

my_access: my_access.c
	gcc -o my_access my_access.c
	chown test_user my_access
	chmod u+s my_access

run_test_chattr: my_chattr my_chattr.dat
	ansible-playbook setup_test_chattr.yml
#	sudo ./run_test_chattr.sh

%: %.c
	gcc -o $@ $^ 
