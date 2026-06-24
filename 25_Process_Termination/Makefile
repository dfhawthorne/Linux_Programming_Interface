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

all: test_1

test_1: Ex_1
	./Ex_1 $(RUN_ARGS)

%: %.c
	$(CC) $(CFLAGS) $^ -o $@
