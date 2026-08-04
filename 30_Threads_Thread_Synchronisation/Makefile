.PHONY: all test_1 test_2

# ------------------------------------------------------------------------------
# Set up global variables
# ------------------------------------------------------------------------------

UNAME_S := $(shell uname -s)
INC_DIR := ../03_System_Programming_Concepts
LIB_DIR := ../Shared_Libraries
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

test_1: thread_incr
	./Ex_1.sh

test_2: Ex_2
	./Ex_2.sh

thread_incr: thread_incr.c
	$(CC) $(CFLAGS) -I$(INC_DIR) $^ -L$(LIB_DIR) -ltlpi_base -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

libunbalanced_tree.so: unbalanced_tree.o
	$(CC) -shared -o $@ $^

Ex_2: Ex_2.c libunbalanced_tree.so
	$(CC) $(CFLAGS) $< -L. -lunbalanced_tree -o $@
