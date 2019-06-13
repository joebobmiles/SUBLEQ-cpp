/**
 * @file main.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-06-10
 *
 * This file contains the main entry point for subleq.exe, the SUBLEQ
 * instruction set VM/interpreter.
 */

// C standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv)
{
    FILE *Binary = fopen(argv[1], "r");
    assert(Binary != 0);

    // Discover binary size.
    fseek(Binary, 0, SEEK_END);
    long Size = ftell(Binary);
    rewind(Binary);

    // FIXME[joe] Bad practice, but I'm pretty sure Size is in bytes.
    // NOTE[joe] We may not need to malloc this memory...
    int *Program = (int *)malloc(Size);
    assert(Program != 0);

    // TODO[joe] Abort if we haven't read the bytes we are looking for?
    fread(Program,
          sizeof(int),
          Size / sizeof(int),
          Binary);

    int ProgramCounter = 0;

    do
    {
        int *A = Program + Program[ProgramCounter++];
        int *B = Program + Program[ProgramCounter++];
        // @NOTE[joe] While A, B, and C are technically addresses, C is fairly
        // useless as a pointer. This is mostly because it's used to control
        // the program counter and not used to do math. Because the program
        // counter is just an offset into the program and C is supposed to be
        // the same, skipping the translation of C into a true pointer is
        // natural.
        int  C = Program[ProgramCounter++];

        // The SUBLEQ operation.
        if ((*B = *A - *B) <= 0) ProgramCounter = C;

        printf("%d\n", *B);
    }
    // If we read a negative offset for our next offset, the program ends.
    while (ProgramCounter >= 0);

    return 0;
}
