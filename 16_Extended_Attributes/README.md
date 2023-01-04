# Chapter 16: Extended Attributes

## Table of Contents

  * [Exercise 16-1](#exercise-16-1)


## Exercise 16-1

Run:
```bash
make all
```
The expected output is:
```
rm my_setfattr xattr_view
gcc -o my_setfattr my_setfattr.c 
gcc -o xattr_view xattr_view.c 
touch test_file
./my_setfattr -n user.x -v "The past is not dead" test_file
./my_setfattr -n user.y -v "In fact, it is not even past" test_file
./xattr_view test_file
test_file:
        name=user.y; value=In fact, it is not even past
        name=user.x; value=The past is not dead

./my_setfattr -n user.x test_file
user.x='The past is not dead'
./my_setfattr -x user.x test_file
./xattr_view test_file
test_file:
        name=user.y; value=In fact, it is not even past
```