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

// @NOTE[joe] Z is used as a symbolic value for the address that stores 0.
#define Z 0

int main(void)
{
    int Program[] = {
        // BEGIN Instructions.
        6, 7, -1,
        0, 0, -1, // End of program tryptich.
        // BEGIN Data.
        1, -1, 0
    };

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
    while (ProgramCounter > 0);

    return 0;
}
