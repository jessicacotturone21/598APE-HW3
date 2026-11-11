# CS598APE HW3

This repository contains code for homework 3 of CS598APE. It should not need any modifications to run.

`sandpile.c` contains the implementation of finding the identity element of a 2D
abelian sandpile.

![Identity sandpile on a 1001x1001 pacman grid](identity-ref.png)

*Identity sandpile on a 1001x1001 pacman grid*


To compile the program, run:

```bash
make all
```

To clean existing build artifacts, run:

```bash
make clean
```

Usage (after compiling):

```bash
./sandpile.exe <N> 
```

where `N` is the grid size

Consider speeding up the generation of this 1001x1001 identity sandpile.

```bash
./sandpile.exe 1001
```

## Optimizations
Opt 1: Parallelization, commit `4f2daec`
Opt 2: Narrowing the data type, commit `e212627`
Opt 3: Hoisting a loop-invariant load, commits `09ece5c` and `0e52b85`
Opt 4: Branch removal, commit `2da6f67`
Opt 5: Toppling more than once, commit `48f5407`
