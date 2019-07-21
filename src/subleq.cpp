/**
 * @file subleq.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-06-10
 *
 * This file contains the main entry point for subleq.exe, the SUBLEQ emulator.
 */

// C standard libraries.
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <fstream>


int main(int argc, char** argv)
{
    std::ifstream BinaryFile (argv[1],
                              std::ifstream::out | std::ifstream::binary);
    // TODO[joe] Repor that we weren't given an input file.
    assert(BinaryFile);

    // Discover binary size.
    BinaryFile.seekg(0, BinaryFile.end);
    long FileSize = BinaryFile.tellg();
    BinaryFile.seekg(0, BinaryFile.beg);

    int *Program = new int[FileSize];

    // TODO[joe] Abort if we haven't read the bytes we are looking for?
    BinaryFile.read((char *)Program, FileSize);


    int ProgramCounter = 0;

    // TODO[joe] Implement bounds checking so that the user can't access memory
    // outside the program.
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

    // TODO[joe] Perform error reporting here? If we have an out of bounds PC,
    // we more than likely will break out of the execution loop and wind up
    // here.

    return 0;
}
