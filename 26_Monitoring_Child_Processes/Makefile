.PHONY: all test_1 test_2 test_3 test_4 test_5

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

all: test_1 test_2 test_3 test_4

test_1: Ex_1
	./Ex_1 $(RUN_ARGS)

test_2: Ex_2
	./Ex_2 $(RUN_ARGS)

test_3: child_status
	./child_status $(RUN_ARGS)

test_4: make_zombie
	./make_zombie $(RUN_ARGS)

child_status: child_status.c print_wait_status.c error_functions.c get_num.c
	$(CC) $(CFLAGS) $^ -o $@

make_zombie: make_zombie.c error_functions.c
	$(CC) $(CFLAGS) $^ -o $@

%: %.c
	$(CC) $(CFLAGS) $^ -o $@
