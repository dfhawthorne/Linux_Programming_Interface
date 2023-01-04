# ------------------------------------------------------------------------------
# Make file for Chapter 16: Extended Attributes
# ------------------------------------------------------------------------------

.PHONY : all clean test
all : clean my_setfattr xattr_view test

clean :
	rm my_setfattr xattr_view

# Based on p.312

test:
	touch test_file
	./my_setfattr -n user.x -v "The past is not dead" test_file
	./my_setfattr -n user.y -v "In fact, it is not even past" test_file
	./xattr_view test_file
	./my_setfattr -n user.x test_file
	./my_setfattr -x user.x test_file
	./xattr_view test_file

%: %.c
	gcc -o $@ $^ 