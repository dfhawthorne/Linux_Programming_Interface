.PHONY: all test_1 test_2

# ------------------------------------------------------------------------------
# Set up global variables
# ------------------------------------------------------------------------------

UNAME_S := $(shell uname -s)
CC      ?= gcc
CSTD    ?= -std=c17
WARN    ?= -Wall -Wextra -Wpedantic -Wimplicit-fallthrough
OPT     ?= -O2
THREADS ?= -pthread -D_REENTRANT

MY_CFLAGS := $(CSTD) $(WARN) $(OPT) $(THREADS) -D_GNU_SOURCE

CFLAGS  += $(MY_CFLAGS)

RUN_ARGS :=

ifeq ($(VERBOSE),1)
RUN_ARGS += -v
endif


# ------------------------------------------------------------------------------
# All test cases
# ------------------------------------------------------------------------------

all: test_1 test_2

test_1: Ex_1 Ex_1a
	./Ex_1.sh

test_2: Ex_2
	./Ex_2

%: %.c
	$(CC) $(CFLAGS) $^ -o $@
