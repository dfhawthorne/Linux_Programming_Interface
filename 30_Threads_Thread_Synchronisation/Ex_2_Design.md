# Exercise 30-2 Design Document

## Summary

This document records design decisions made during the solution of Exercise 30-2.

## Table of Contents

* [Naming Conventions](#naming-conventions)
* [Use of Shared Library](#use-of-shared-library)
* [Mutex Hierarchy](#mutex-hierarchy)
  * [Inter-Nodal Hierarchy](#inter-nodal-hierarchy)
  * [Intra-Nodal Hierarchy](#intra-nodal-hierarchy)
* [Error Handling](#error-handling)

## Naming Conventions

I chose `unbalanced_tree` for the name of the header file, source code, and
library name as I thought it was most evocative. I chose to prefix the
library functions by `utree_` in order to overcome the VS Code highlighting
of C++ keywords such as `destroy`. I think the shortened prefix complements
the file names. Also, `utree` stands with `btree` which is a common abbreviation
for balanced trees.

## Use of Shared Library

I decided to create a shared library called `libunbalanced_tree.so` that
implements the library routines.

To use a single source file risks the temptation of reduced library code
discipline. That is, implementation could assume that the library code
manages the state information. The best solution is that the state is
managed by the calling program.

## Mutex Hierarchy

I propose two levels to the mutex hierarchy:

1. Inter-nodal hierarchy
1. Intra-nodal hierarchy

### Inter-Nodal Hierarchy

The parent mutex should always be acquired before the child's.

### Intra-Nodal Hierarchy

Within a node, the mutexes should be acquired in the following order:

1. Node
1. Left-branch
1. Right-branch

## Error Handling

Instead of using `errno` or a bespoke error handling, I plan to follow the
`pthreads` model with error codes passed back as return values. I tried
to use the standard `errno` values, but found them to be incomplete.

These return values will use the standard values from `errno`:

| Symbolic Error Name | ERRNO | Meaning | Comments |
| --- | ---: | --- | --- |
| ENOENT | 2 | No such file or directory | Closest fit for `utree_destroy()` not finding the key in the tree |
| ENOMEM | 12 | Cannot allocate memory | |
| EEXIST | 17 | File exists | Closest fit for `utree_add()` finding the key already in the tree. |
| EINVAL | 22 | Invalid argument | Used for checking argument to function calls |
| ENOSYS | 38 | Function not implemented | Used during library development |
| EALREADY | 114 | Operation already in progress | For duplicate calls to `utree_destroy()` |

__NOTE__: `ENOENT` and `EEXIST` references _file_ instead of _key_.

__NOTE__: Since this is a quick and dirty implementation, I will use the
standard `errno` values, instead of creating a bespoke error list and
associated functions, such as a `strerror` equivalent.
