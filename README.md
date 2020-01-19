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

If you know what you're doing, that maybe all you need. For those who want to
use the existing Windows tooling, you will need:

 -  LLVM clang v9.0+
 -  Microsoft Visual Studio 2017
 -  PowerShell v3+ (if using PowerShell)

## Building on Windows

### With CMD

Make sure that the Visual Studio devtools is in your system path (requires
Visual Studio installed). You can use `shell.bat` in the scripts directory to
do this. Then use `build.bat` from the project root to build `subleq` and
`subleqc`.

### With PowerShell

Run `build.ps1` from the project root. You can use `scripts.ps1` to make the
build script a global command, but it will only work in the project root.

# References

Mazonka O, Kolodin A. 2011. [_A simple multi-processor computer based on
subleq_](https://arxiv.org/ftp/arxiv/papers/1106/1106.2593.pdf).
