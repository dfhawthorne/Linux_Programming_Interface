# ------------------------------------------------------------------------------
# Makefile for chapter 20: Signals: Fundamental Concepts
# ------------------------------------------------------------------------------

.PHONY: all all_v clean test test_v \
        test_1_n test_1_n_v test_1_s test_1_s_v \
		test_2 test_2_v \
		test_3 test_3_v \
		test_4 test_4_v

all: clean test
all_v: clean test_v

clean:
	rm -fR test*

# ------------------------------------------------------------------------------
# Run test cases
# ------------------------------------------------------------------------------

test_%_v: verbose_mode ::= --verbose
test_1_s test_1_s_v: sleep_time ::= "--sleep-time=5"
test:   test_1_n   test_1_s   test_2   test_3   test_4
test_v: test_1_n_v test_1_s_v test_2_v test_3_v test_4_v

test_1_n test_1_n_v test_1_s test_1_s_v: sig_receiver sig_sender
	./run_test_1.sh ${verbose_mode} ${sleep_time}

test_2 test_2_v: sig_ignore
	./run_test_2.sh ${verbose_mode}

test_3 test_3_v: sig_action
	./run_test_3.sh ${verbose_mode}

test_4 test_4_v: sig_interrupt
	./run_test_4.sh ${verbose_mode}

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 