.PHONY: all clean test test_v test_1 test_1_v

all: clean test

clean:
	rm -f test*

test_%_v: \
	verbose_mode ::= --verbose

test: test_1
test_v: test_1_v

test_1 test_1_v: Ex_22_1
	./Ex_22_1.sh ${verbose_mode} --log-dir=logs/Ex_22_1_nomask
	./Ex_22_1.sh ${verbose_mode} --mask --log-dir=logs/Ex_22_1_mask

# ------------------------------------------------------------------------------
# Compile programs
# ------------------------------------------------------------------------------

%: %.c
	gcc -o $@ $^ 