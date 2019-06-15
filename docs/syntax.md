SUBLEQ Assembly Syntax
======================

## General Introduction

If SUBLEQ assembly was to be written in a similar fashion to other assembly
languages like x86 or ARM, it would look something like this:

```
SUBLEQ 1, B, C
SUBLEQ B, F, D
SUBLEQ D, F, L
```

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
