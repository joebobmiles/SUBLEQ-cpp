SUBLEQ Assembly Syntax
======================

## General Introduction

If SUBLEQ assembly was to be written in a similar fashion to other assembly
languages like x86 or ARM, it would look something like this:

```
SUBLEQ A, B, C
SUBLEQ B, F, D
SUBLEQ D, F, L
```

Where each operand given to the SUBLEQ instruction is an address in memory.

As you can see, on each line we write the name of the operation, followed by
the operands that the operation is to be performed on. For something like
SUBLEQ, not only is this exhausting, it's unnecessary. Instead, most SUBLEQ
implementations write their assembly like this:

```
A, B, C
B, F, D
D, F, L
```

For the SUBLEQ assembler in this project, we use white space to delimit
operands and newlines to delimit instructions. This means the above becomes:

```
A B C
B F D
D F L
```

## Memory Addressing

While in the above example we used letters to represent addresses in memory,
the actual SUBLEQ assembler (for now) does not understand symbolic names for
memory locations. Instead, a SUBLEQ assembly program uses integer values that
represent the offset from the first cell in memory. Thus, a SUBLEQ program
would more resemble:

```
0 1 3
0 1 6
0 0 -1
```

This program subtracts the contents of cell 1 (1) and the contents of cell 0
(0) and stores the result (-1) in cell 1, branching if the result is negative
to cell 3 (the next instruction). The second instruction repeats the first
(though the result is now 1) and branches to cell 6 if the result is negative.
Cell 6 is the beginning of the third instruction, which subtracts the contents
of Cell 0 from Cell 0, and stores the result (0) there, branching into the
memory address of -1 - an exit condition.

### Special Memory Addresses

| Address | Name | Usage |
|:-------:|:----:|:------|
|    -1   | `sysout` | This address is used as an exit condition. Branching to this address halts execution of the SUBLEQ program. |

## Next Address Operator

When writing a SUBLEQ program, it's useful to "suppress" branching if the
result of a SUBLEQ is less than or equal to zero. This can be done by telling
the SUBLEQ command to instead branch to the next cell in memory, which would be
the beginning of the next SUBLEQ command. This can be tedious to do by hand, so
the SUBLEQ assembler has a built in operator - the "next address" operator or
`?`. For an example:

```
0 1 ?
0 1 ?
0 0 -1
```

This performs SUBLEQ on the cells 0 and 1 twice before exiting. (The effect of
this inverts the value in cell 1 from 1 to -1 and back again.) This operator
also has a few other uses, namely subtracting the value at an address from
itself (`? 1 ?`) and changing the value of a branch address (`1 ? 3`).
