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
SUBLEQ, not only is this exhausting, it's unnecessary. Instead, SUBLEQ assembly
is written like this:

```
A, B, C
B, F, D
D, F, L
```

Additionally, the SUBLEQ assembler recognizes newlines and semicolons as
end-of-line (EOL) characters, so the above could also be written as:

```
A, B, C; B, F, D; D, F, L;
```

## Memory Addressing

While in the above example we used letters to represent addresses in memory,
the actual SUBLEQ assembler (for now) does not understand symbolic names for
memory locations. Instead, a SUBLEQ assembly program uses integer values that
represent the offset from the first cell in memory. Thus, a SUBLEQ program
would more resemble:

```
0, 1, 3
0, 1, 6
0, 0, -1
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
the beginning of the next SUBLEQ command. This can be taxing on the programmer
if they had to keep track of what address was the next one. To simplify this,
the SUBLEQ assembler has a built-in operator for representing the next address:
`?`. Here's what our example code looks like with the next address operator:

```
0, 1, ?
0, 1, ?
0, 0, -1
```

This performs SUBLEQ on the cells 0 and 1 twice before exiting. (The effect of
this inverts the value in cell 1 from 1 to -1 and back again.) This operator
also has a few other uses, namely subtracting the value at an address from
itself (`?, 1, ?`) and changing the value of a branch address (`1, ?, 3`).

## Instruction Notation

In SUBLEQ, there are three different forms for writing a SUBLEQ instruction.
The one shown so far will be referred to as _normal_ or _three-address_
notation. The other two forms of writing SUBLEQ instructions are illustrated
below.

### Two-Address Notation

Two-address notation is, as the name suggests, writing a SUBLEQ instruction
with only two addresses. While it may seem odd to write an instruction that
requires 3 addresses with 2, let's look at the previous example code:

```
0, 1, ?
0, 1, ?
0, 0, -1
```

The first thing to notice is that we use the next address operator twice. While
not necessarily an egregious waste of effort, it can get repetitive when
writing a series of arithmetic operations that must be executed in order. This
is where two-address notation comes in. Two-address notation takes an
instruction like `0, 1, ?` and drops the last parameter, making it `0, 1`.
In the assembler, `0, 1` is understood to mean `0, 1, ?` and the assembler will
add in the missing operator.

This makes the above example code now look like:

```
0, 1
0, 1
0, 0, -1
```

### One-Address Notation

What seems stranger than two-address notation is one-address notation. As its
name suggests, one-address notation is writing a SUBLEQ instruction with only
one address. Written as simply `A`, an instruction written in one-address
notation is expanded into `A, A, ?` by the assembler. The use of this notation
is usually for the common operation of "initializing" an address to 0 and
continuing with execution.
