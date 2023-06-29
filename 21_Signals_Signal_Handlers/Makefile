.PHONY: all clean test_abort

all: clean test_abort

clean:
	rm -f my_abort

test_abort: my_abort
	./run_test_abort.sh

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 
