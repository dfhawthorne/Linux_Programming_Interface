.PHONY: all test_1 test_2 test_3 test_4

all: test_1 test_2 test_3 test_4

test_1: Ex_23_1
	./Ex_23_1

test_2: t_nanosleep Ex_23_2 t_clock_nanosleep
	./Ex_23_2.sh

test_3: Ex_23_3
	./Ex_23_3
	./Ex_23_3 EVP

test_4: ptmr_sigev_thread
	./ptmr_sigev_thread 60

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 

t_nanosleep: t_nanosleep.c error_functions.c get_num.c
	gcc -o t_nanosleep t_nanosleep.c error_functions.c get_num.c

t_clock_nanosleep: t_clock_nanosleep.c error_functions.c get_num.c
	gcc -o t_clock_nanosleep t_clock_nanosleep.c error_functions.c get_num.c

Ex_23_2: Ex_23_2.c error_functions.c get_num.c
	gcc -o Ex_23_2 Ex_23_2.c error_functions.c get_num.c

ptmr_sigev_thread: ptmr_sigev_thread.c
	gcc -o ptmr_sigev_thread ptmr_sigev_thread.c error_functions.c get_num.c itimerspec_from_str.c curr_time.c
