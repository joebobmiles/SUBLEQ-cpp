SUBLEQ Machine
==============

This is a project to create a
[SUBLEQ](https://en.wikipedia.org/wiki/One_instruction_set_computer#Subtract_and_branch_if_less_than_or_equal_to_zero)
emulator, and an accompanying assembler for SUBLEQ assembly, in C(++). The
primary goal is to create a program that can accept a binary file as input and
produce output by interpreting the SUBLEQ instructions contained in the binary.
The assembler converts a human-readable and -writeable instruction format into
this binary format.

# Theory of Operation

## The `subleq` Command

The `subleq` command is a SUBLEQ emulator. It takes as input a binary file that
contains only 32-bit integers. This binary file is the program that the
emulator runs. Starting at the first integer (offset 0 in the binary file), the
emulator runs the SUBLEQ algorithm. If the algorithm attempts to access an
unknown or out of bounds offset, it will report an error and halt. If the
emulator branches to an offset of -1, it will halt. If no binary file is given
as input, the emulator will raise an error identifying the problem and halt.

## The `subleqc` Command

The `subleqc` command is a SUBLEQ assembler. It takes as input a binary file
that contains ASCII plain-text. The input text should be recognizable by the
[SUBLEQ grammar](docs/Grammar.md) that `subleqc` implements. If the input text
cannot be recognized by the implemented grammar, then the assembler will report
an error and halt. The text will be tokenized according to the implemented
grammar and transformed into a stream of integers. This stream of integers will
be written to a binary file, whose file name is provided by the user. If no
input file name or output file name is given to the assembler, it will raise an
error identifying the problem and halt.

# Building the Project

You need the following to build this project:
 -  A C++ compiler.

To run the build on a Windows machine, make sure you have Visual Studio
devtools in your system path and run the build script found in the scripts
folder.

# References

Mazonka O, Kolodin A. 2011. [_A simple multi-processor computer based on
subleq_](https://arxiv.org/ftp/arxiv/papers/1106/1106.2593.pdf).
