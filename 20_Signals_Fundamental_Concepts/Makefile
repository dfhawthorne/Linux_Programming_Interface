# ------------------------------------------------------------------------------
# Makefile for chapter 20: Signals: Fundamental Concepts
# ------------------------------------------------------------------------------

.PHONY: all all_v clean test test_v \
        test_1_n test_1_n_v test_1_s test_1_s_v \
		test_2 test_2_v \
		test_3_r test_3_r_v test_3_n test_3_n_v \
		test_4 test_4_v

all: clean test
all_v: clean test_v

clean:
	rm -fR test*

# ------------------------------------------------------------------------------
# Run test cases
# ------------------------------------------------------------------------------

test_%_v: \
	verbose_mode ::= --verbose
test_1_s test_1_s_v \
test_2 test_2_v: \
	sleep_time   ::= "--sleep-time=5"
test_1_n test_1_n_v \
test_1_s test_1_s_v: \
	log_dir      ::= "--log-dir=logs/test1"
test_2 test_2_v: \
	ignore_mode  ::= "--ignore=10"
	log_dir      ::= "--log-dir=logs/test2"
test_3_r test_3_r_v: \
	resethand    ::= "--reset-hand"
test_3_n test_3_n_v: \
	nodefer      ::= "--nodefer"
test_3_r test_3_r_v \
test_3_n test_3_n_v: \
	log_dir      ::= "--log-dir=logs/test3"
	slow_signal  ::= "--slow-signal"

test:   test_1_n   test_1_s   test_2   test_3_r   test_3_n   test_4
test_v: test_1_n_v test_1_s_v test_2_v test_3_r_v test_3_n_v test_4_v

test_1_n test_1_n_v test_1_s test_1_s_v \
test_2 test_2_v \
test_3_r test_3_r_v test_3_n test_3_n_v: sig_receiver sig_sender
	./run_test_1.sh ${verbose_mode} ${sleep_time} ${ignore_mode} ${log_dir} ${resethand} ${nodefer} ${slow_signal}

test_4 test_4_v: sig_interrupt
	./run_test_4.sh ${verbose_mode}

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 