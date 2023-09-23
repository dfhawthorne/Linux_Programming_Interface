.PHONY: all clean test test_v \
		test_1 test_1_v \
		test_2 test_2_v \
		test_3 test_3_v

all: clean test

clean:
	rm -f test*

test_%_v: \
	verbose_mode ::= --verbose

test: test_1 test_2 test_3
test_v: test_1_v test_2_v test_3_v

test_1 test_1_v: Ex_22_1
	./Ex_22_1.sh ${verbose_mode} --log-dir=logs/Ex_22_1_nomask
	./Ex_22_1.sh ${verbose_mode} --mask --log-dir=logs/Ex_22_1_mask

test_2 test_2_v: Ex_22_2
	./Ex_22_2.sh ${verbose_mode} --log-dir=logs/Ex_22_2

test_3 test_3_v: Ex_22_3
	./Ex_22_3.sh ${verbose_mode} --log-dir=logs/Ex_22_3

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 