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

all: test_1 test_2 test_3 test_4 test_5

test_1:
	@echo "There are eight processes created:"
	@echo "Parent executes fork()"
	@echo "Parent and child_1 execute fork()"
	@echo "Parent, child_1, child_2, and child_1_1 execute fork()"
	@echo "Now Parent, child_1, child_2, child_3, child_1_1, child_1_2, child_2_1, and child_1_1_1 are executing."
	@echo ""
	@echo "The process tree is:"
	@echo "Parent +-> child_1 +-> child_1_1 -> child_1_1_1"
	@echo "       |           +-> child_1_2"
	@echo "       +-> child_2  -> child_2_1"
	@echo "       +-> child_3"

test_2: Ex_2
	./Ex_2 $(RUN_ARGS)

test_3: Ex_3
	ulimit -c unlimited
	./Ex_3 $(RUN_ARGS)

test_4:
    ifeq ("$(strip $(UNAME_S))","Linux")
		$(warning test_4 should not be run on Linux)
    else
		$(error test_4 is not implemented on any other platforms)
    endif
	@echo ""

test_5: Ex_5
	./Ex_5

Ex_5: Ex_5.c curr_time.c get_num.c error_functions.c
	$(CC) $(CFLAGS) $^ -o $@

%: %.c
	$(CC) $(CFLAGS) $^ -o $@
