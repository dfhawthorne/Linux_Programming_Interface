# ------------------------------------------------------------------------------
# Make file for Ch 19: Monitoring File Events
# ------------------------------------------------------------------------------

.PHONY : all all_v clean clean_pgm clean_data test test_v

all: clean_data test

all_v: clean_data test_v

# ------------------------------------------------------------------------------
# Clean up is split between programs and data
# ------------------------------------------------------------------------------

clean: clean_pgm clean_data

clean_pgm:
	rm -f mon_dir_evt

clean_data:
	rm -fR test_dir
	rm -fR test_other
	rm -f test.log
	rm -f test.err
	rm -f test.pid

# ------------------------------------------------------------------------------
# Test can be done with or without verbose output (-v)
# ------------------------------------------------------------------------------

test: mon_dir_evt
	./run_test.sh

test_v: mon_dir_evt
	./run_test.sh -v

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 