# 24 Process Creation

## Exercise 24-1

Run:

```bash
make test_1
```

The sample output is:

```text
There are eight processes created:
Parent executes fork()
Parent and child_1 execute fork()
Parent, child_1, child_2, and child_1_1 execute fork()
Now Parent, child_1, child_2, child_3, child_1_1, child_1_2, child_2_1, and child_1_1_1 are executing.

The process tree is:
Parent +-> child_1 +-> child_1_1 -> child_1_1_1
       |           +-> child_1_2
       +-> child_2  -> child_2_1
       +-> child_3
```
