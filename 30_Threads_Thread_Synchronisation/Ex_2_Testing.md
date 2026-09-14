# Exercise 30-2 Testing Document

## Summary

This document describes the detailed testing plan for the Unbalanced Tree
library.

## Table of Contents

- [Test Data](#test-data)
- [Test Trees](#test-trees)
  - [Balanced Tree](#balanced-tree)
- [Test Case #1: Argument Validation](#test-case-1-argument-validation)
- [Test Case #2: Searching Empty Trees](#test-case-2-searching-empty-trees)

## Test Data

I will keep the testing simple with a small set of data. I propose the
following:

| Key | Value |
| --- | --- |
| Apple | Fruit |
| Banana | Berry |
| Cat | Animal |
| Diamond | Mineral |
| Echo | Sound |
| Fox | Animal |
| Golf | Game |

## Test Trees

I will use three (3) types of trees:

- [Balanced Tree](#balanced-tree)
- [Left-Skewed Binary Tree](#left-skewed-binary-tree)
- [Right-Skewed Binary Tree](#right-skewed-binary-tree)

These three (3) tree types test the canonical balanced tree, as well as the
two (2) pathological trees. This gives a wide spectrum of tree types.

### Balanced Tree

A balanced tree using the following key insertion order:

1. Diamond
1. Banana
1. Apple
1. Cat
1. Fox
1. Echo
1. Golf

### Left-Skewed Binary Tree

A tree with only nodes on left links. The tree is constructed with keys
inserted in the following order:

1. Golf
1. Fox
1. Echo
1. Diamond
1. Cat
1. Banana
1. Apple

### Right-Skewed Binary Tree

A tree with only nodes on right links. The tree is constructed with keys
inserted in the following order:

1. Apple
1. Banana
1. Cat
1. Diamond
1. Echo
1. Fox
1. Golf

## Test Case #1: Argument Validation

All of the following calls should fail with `EINVAL`:

1. `utree_initialise(NULL)`
1. `utree_add(NULL, NULL, NULL)`
1. `utree_add(NULL, key, NULL)`
1. `utree_add(NULL, NULL, value)`
1. `utree_add(root_p, NULL, NULL)`
1. `utree_add(root_p, key, NULL)`
1. `utree_add(root_p, NULL, value)`
1. `utree_delete(NULL, NULL)`
1. `utree_delete(root_p, NULL)`
1. `utree_delete(NULL, key)`

## Test Case #2: Searching Empty Trees

An empty tree is created, then three (3) threads are created to search for two
keys each. Each call should return value as a `NULL` pointer.
