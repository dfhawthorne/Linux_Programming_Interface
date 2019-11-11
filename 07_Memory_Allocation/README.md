# Chapter 7: Memory Allocation

## Exercise 7-1

The source code is downloaded from [free_and_sbrk.c](http://man7.org/tlpi/code/online/dist/memalloc/free_and_sbrk.c.html).

To do the exercise:
1. Use `make free_and_sbrk` to compile and link the code.
1. Use `./free_and_sbrk 1000 10240 2` to execute.

The output should be similar to:
```
Initial program break:          0x560e940a8000
Allocating 1000*10240 bytes
Current prog break after #  13: 0x560e940cb000 (Increase of 0X0000023000)
Current prog break after #  26: 0x560e940ec000 (Increase of 0X0000021000)
Current prog break after #  40: 0x560e9410f000 (Increase of 0X0000023000)
Current prog break after #  54: 0x560e94132000 (Increase of 0X0000023000)
Current prog break after #  68: 0x560e94155000 (Increase of 0X0000023000)
Current prog break after #  82: 0x560e94178000 (Increase of 0X0000023000)
Current prog break after #  96: 0x560e9419b000 (Increase of 0X0000023000)
Current prog break after # 110: 0x560e941be000 (Increase of 0X0000023000)
Current prog break after # 124: 0x560e941e1000 (Increase of 0X0000023000)
Current prog break after # 138: 0x560e94204000 (Increase of 0X0000023000)
Current prog break after # 152: 0x560e94227000 (Increase of 0X0000023000)
Current prog break after # 165: 0x560e94248000 (Increase of 0X0000021000)
Current prog break after # 179: 0x560e9426b000 (Increase of 0X0000023000)
Current prog break after # 193: 0x560e9428e000 (Increase of 0X0000023000)
Current prog break after # 207: 0x560e942b1000 (Increase of 0X0000023000)
Current prog break after # 221: 0x560e942d4000 (Increase of 0X0000023000)
Current prog break after # 235: 0x560e942f7000 (Increase of 0X0000023000)
Current prog break after # 249: 0x560e9431a000 (Increase of 0X0000023000)
Current prog break after # 263: 0x560e9433d000 (Increase of 0X0000023000)
Current prog break after # 277: 0x560e94360000 (Increase of 0X0000023000)
Current prog break after # 290: 0x560e94381000 (Increase of 0X0000021000)
Current prog break after # 304: 0x560e943a4000 (Increase of 0X0000023000)
Current prog break after # 318: 0x560e943c7000 (Increase of 0X0000023000)
Current prog break after # 332: 0x560e943ea000 (Increase of 0X0000023000)
Current prog break after # 346: 0x560e9440d000 (Increase of 0X0000023000)
Current prog break after # 360: 0x560e94430000 (Increase of 0X0000023000)
Current prog break after # 374: 0x560e94453000 (Increase of 0X0000023000)
Current prog break after # 388: 0x560e94476000 (Increase of 0X0000023000)
Current prog break after # 402: 0x560e94499000 (Increase of 0X0000023000)
Current prog break after # 415: 0x560e944ba000 (Increase of 0X0000021000)
Current prog break after # 429: 0x560e944dd000 (Increase of 0X0000023000)
Current prog break after # 443: 0x560e94500000 (Increase of 0X0000023000)
Current prog break after # 457: 0x560e94523000 (Increase of 0X0000023000)
Current prog break after # 471: 0x560e94546000 (Increase of 0X0000023000)
Current prog break after # 485: 0x560e94569000 (Increase of 0X0000023000)
Current prog break after # 499: 0x560e9458c000 (Increase of 0X0000023000)
Current prog break after # 513: 0x560e945af000 (Increase of 0X0000023000)
Current prog break after # 527: 0x560e945d2000 (Increase of 0X0000023000)
Current prog break after # 540: 0x560e945f3000 (Increase of 0X0000021000)
Current prog break after # 554: 0x560e94616000 (Increase of 0X0000023000)
Current prog break after # 568: 0x560e94639000 (Increase of 0X0000023000)
Current prog break after # 582: 0x560e9465c000 (Increase of 0X0000023000)
Current prog break after # 596: 0x560e9467f000 (Increase of 0X0000023000)
Current prog break after # 610: 0x560e946a2000 (Increase of 0X0000023000)
Current prog break after # 624: 0x560e946c5000 (Increase of 0X0000023000)
Current prog break after # 638: 0x560e946e8000 (Increase of 0X0000023000)
Current prog break after # 652: 0x560e9470b000 (Increase of 0X0000023000)
Current prog break after # 665: 0x560e9472c000 (Increase of 0X0000021000)
Current prog break after # 679: 0x560e9474f000 (Increase of 0X0000023000)
Current prog break after # 693: 0x560e94772000 (Increase of 0X0000023000)
Current prog break after # 707: 0x560e94795000 (Increase of 0X0000023000)
Current prog break after # 721: 0x560e947b8000 (Increase of 0X0000023000)
Current prog break after # 735: 0x560e947db000 (Increase of 0X0000023000)
Current prog break after # 749: 0x560e947fe000 (Increase of 0X0000023000)
Current prog break after # 763: 0x560e94821000 (Increase of 0X0000023000)
Current prog break after # 777: 0x560e94844000 (Increase of 0X0000023000)
Current prog break after # 791: 0x560e94867000 (Increase of 0X0000023000)
Current prog break after # 804: 0x560e94888000 (Increase of 0X0000021000)
Current prog break after # 818: 0x560e948ab000 (Increase of 0X0000023000)
Current prog break after # 832: 0x560e948ce000 (Increase of 0X0000023000)
Current prog break after # 846: 0x560e948f1000 (Increase of 0X0000023000)
Current prog break after # 860: 0x560e94914000 (Increase of 0X0000023000)
Current prog break after # 874: 0x560e94937000 (Increase of 0X0000023000)
Current prog break after # 888: 0x560e9495a000 (Increase of 0X0000023000)
Current prog break after # 902: 0x560e9497d000 (Increase of 0X0000023000)
Current prog break after # 916: 0x560e949a0000 (Increase of 0X0000023000)
Current prog break after # 929: 0x560e949c1000 (Increase of 0X0000021000)
Current prog break after # 943: 0x560e949e4000 (Increase of 0X0000023000)
Current prog break after # 957: 0x560e94a07000 (Increase of 0X0000023000)
Current prog break after # 971: 0x560e94a2a000 (Increase of 0X0000023000)
Current prog break after # 985: 0x560e94a4d000 (Increase of 0X0000023000)
Current prog break after # 999: 0x560e94a70000 (Increase of 0X0000023000)
Program break is now:           0x560e94a70000
Freeing blocks from 1 to 1000 in steps of 2
After free(), program break is: 0x560e94a70000
```

## Exercise 7-2

The source code is downloaded from [free_and_sbrk.c](http://man7.org/tlpi/code/online/dist/memalloc/free_and_sbrk.c.html). The modified version is in `my_free_and_sbrk.c`.

To do the exercise:
1. Use `make my_free_and_sbrk` to compile and link the code.
1. Use `./my_free_and_sbrk 10 1024 1 999` to execute.

The output should be similar to:
```

Initial program break:          0x55bcf4d18000
Allocating 10*1024 bytes
Current prog break after #   0: 0x55bcf4d3b000 (Increase of 0X0000023000)
Program break is now:           0x55bcf4d3b000
Freeing blocks from 999 to 10 in steps of 1
After free(), program break is: 0x55bcf4d3b000
```
