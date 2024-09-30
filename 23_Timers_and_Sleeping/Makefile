.PHONY: all test_1 test_2

all: test_1 test_2

test_1: Ex_23_1
	./Ex_23_1

test_2: t_nanosleep Ex_23_2
	./Ex_23_2.sh

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 

t_nanosleep: t_nanosleep.c error_functions.c get_num.c
	gcc -o t_nanosleep t_nanosleep.c error_functions.c get_num.c

Ex_23_2: Ex_23_2.c error_functions.c get_num.c
	gcc -o Ex_23_2 t_nanosleep.c error_functions.c get_num.c
