# Chapter 18: Directories and Links

## Exercise 18-2

The real solution is [here](http://man7.org/tlpi/code/online/dist/dirs_links/bad_symlink.c.html).

My solution is in `ex18_2.c`.

1. Use `make ex18_2` to compile and link the code.
1. Use `./ex18_2` to execute this solution.

## Exercise 18-3

My solution is in `realpath.c`.

1. Use `make realpath` to compile and link this code.
1. Use `./test_realpath.sh` to test this solution.

## Exercise 18-4

My solution is in `list_files.c`.

1. Use `make list_files` to compile and link this code.
1. Use `./list_files` to execute this solution.

__NOTE__: The compilation will give the following messages:
```
list_files.c: In function ‘listFiles’:
list_files.c:42:9: warning: ‘readdir_r’ is deprecated [-Wdeprecated-declarations]
         if ((readdir_r(dirp, dp, &result) != 0) || (result == NULL))
         ^~
In file included from list_files.c:11:0:
/usr/include/dirent.h:183:12: note: declared here
 extern int readdir_r (DIR *__restrict __dirp,
            ^~~~~~~~~
```

## Exercise 18-5

My solution is in `getcwd.c`. (Build completed)

1. Use `make getcwd` to compile and link this program.
1. Use `./getcwd` to execute this solution.
