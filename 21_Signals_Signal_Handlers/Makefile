.PHONY: all clean test_abort_1 test_abort_2

all: clean test_abort_1 test_abort_2

clean:
	rm -f my_abort

# ------------------------------------------------------------------------------
# Test cases for Ex 20-1
# #1: uses abort()
# #2: uses my_abort()
# ------------------------------------------------------------------------------

test_abort_1: \
	use_abort ::= --use-real-abort
	log_file  ::= test1.log

test_abort_2: \
	log_file  ::= test2.log

test_abort_1 test_abort_2: my_abort
	./my_abort --log-file=${log_file} ${use_abort}

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 
