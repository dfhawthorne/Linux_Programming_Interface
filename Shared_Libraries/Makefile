CC = gcc

SRCDIR = ../03_System_Programming_Concepts
INCDIR = ../03_System_Programming_Concepts

CFLAGS = -Wall -Wextra -O2 -fPIC -I$(INCDIR)

.PHONY: all

all: libtlpi_base.so

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

libtlpi_base.so: error_functions.o get_num.o alt_functions.o
	$(CC) -shared -o $@ $^
