# Chapter 11: System Limits and Options

## Exercise 11-1

1. To compile and link this solution, use `make t_sysconf`.
1. To execute this solution, use `./t_sysconf`

The expected output is:
```
_SC_ARG_MAX:         2097152
_SC_LOGIN_NAME_MAX:  256
_SC_OPEN_MAX:        1024
_SC_NGROUPS_MAX:     65536
_SC_PAGESIZE:        4096
_SC_RTSIG_MAX:       32
```

## Exercise 11-2


1. To compile and link this solution, use `make t_fpathconf`.
1. To execute this solution, use `./t_fpathconf`

The expected output is:
```
_PC_NAME_MAX:  255
_PC_PATH_MAX:  4096
_PC_PIPE_BUF:  4096
```
