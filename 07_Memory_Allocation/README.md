# Chapter 7: Memory Allocation

## Exercise 7-1

The source code is downloaded from [free_and_sbrk.c](http://man7.org/tlpi/code/online/dist/memalloc/free_and_sbrk.c.html).

To do the exercise:
1. Use `make free_and_sbrk` to compile and link the code.
1. Use `./free_and_sbrk 1000 10240 2` to execute.

The output should be similar to:
```

Initial program break:          0x55d75a55e000
Allocating 1000*10240 bytes
Program break is now:           0x55d75af26000
Freeing blocks from 1 to 1000 in steps of 2
After free(), program break is: 0x55d75af26000
```

