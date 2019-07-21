Guide to SUBLEQ Compiler
========================

# Usage

To use the SUBLEQ assembler, build subleqc.cpp and invoke the resulting binary
with:

```bash
$ subleqc_binary_name path\to\input_file path\to\output_binary
```

As apparent from above, the SUBLEQ assembler takes to arguments. The first is a
path to a plain text input file that contains SUBLEQ "instructions". The second
is the path into which to output the generated binary. See [Syntax](Syntax.md)
for more information on how to write a SUBLEQ input file.
