.PHONY: all test_1 test_2 test_3 test_4 test_5 test_6

# ------------------------------------------------------------------------------
# Set up global variables
# ------------------------------------------------------------------------------

UNAME_S := $(shell uname -s)
CC      ?= gcc
CSTD    ?= -std=c17
WARN    ?= -Wall -Wextra -Wpedantic -Wimplicit-fallthrough
OPT     ?= -O2

MY_CFLAGS := $(CSTD) $(WARN) $(OPT)

CFLAGS  += $(MY_CFLAGS)

RUN_ARGS :=

ifeq ($(VERBOSE),1)
RUN_ARGS += -v
endif


# ------------------------------------------------------------------------------
# All test cases
# ------------------------------------------------------------------------------

all: test_1 test_2 test_3 test_4 test_5 test_6

test_1: t_execlp dir1/xyz dir2/xyz
	./Ex_1.sh

test_2: Ex_2 Ex_2_child
	./Ex_2 $(RUN_ARGS)

test_3: Ex_3
	./Ex_3 $(RUN_ARGS)

test_4: Ex_4
	./Ex_4 $(RUN_ARGS)

test_5: Ex_5
	./Ex_5 $(RUN_ARGS)

test_6: Ex_6
	./Ex_6 $(RUN_ARGS)

t_execlp: t_execlp.c error_functions.c get_num.c
	$(CC) $(CFLAGS) $^ -o $@

%: %.c
	$(CC) $(CFLAGS) $^ -o $@
