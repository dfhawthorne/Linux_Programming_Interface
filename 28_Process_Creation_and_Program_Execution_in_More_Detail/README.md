# Chapter 28: Process Creation and Program Execution in More Detail

## Exercise 28-1

Run:

```bash
make test_1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_1_fork.c -o Ex_1_fork
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 Ex_1_vfork.c -o Ex_1_vfork
time ./Ex_1_fork
143.05user 159.52system 5:05.86elapsed 98%CPU (0avgtext+0avgdata 1280maxresident)k
0inputs+0outputs (0major+11999948minor)pagefaults 0swaps
time ./Ex_1_vfork
12.58user 16.68system 0:25.92elapsed 112%CPU (0avgtext+0avgdata 1152maxresident)k
0inputs+0outputs (0major+66minor)pagefaults 0swaps
```

Summary results are for 1,000,000 children:

| Method | User Time (s) | System Time (s) | Elapsed |
| --- | ---: | ---: | ---: |
| `fork()` | 143.05 | 159.52 | 5:05.86 |
| `vfork()` | 12.58 | 16.68 | 0:25.92 |

These results still show the order of magnitude difference between the two (2) calls.
