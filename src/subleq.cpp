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


#define Deref(OFFSET)    *(Program + OFFSET)
#define IsStdout(OFFSET) (OFFSET == -1)


enum status {
    NORMAL,
    NO_INPUT,
    NO_SUCH_FILE,
    OFFSET_OUT_OF_BOUNDS,
    UNKNOWN
};


static
bool InBounds(int Offset, long Extent)
{
    if (IsStdout(Offset))
        return true;

    else if (0 <= Offset && Offset < Extent)
        return true;

    else
        return false;
}


int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("No input binary given, exiting.\n");
        return NO_INPUT;
    }


    std::ifstream BinaryFile (argv[1],
                              std::ifstream::out | std::ifstream::binary);
    if (!BinaryFile)
    {
        printf("Failed to open binary \"%s\", exiting.\n", argv[1]);
        return NO_SUCH_FILE; 
    }

    // Discover binary size.
    BinaryFile.seekg(0, BinaryFile.end);
    long ProgramLength = BinaryFile.tellg();
    BinaryFile.seekg(0, BinaryFile.beg);

    int *Program = new int[ProgramLength];

    // TODO[joe] Abort if we haven't read the bytes we are looking for?
    BinaryFile.read((char *)Program, ProgramLength);


    int ProgramCounter, A, B, C = 0;

    do
    {
        A = Program[ProgramCounter++];
        B = Program[ProgramCounter++];
        C = Program[ProgramCounter++];

        if (!InBounds(A, ProgramLength) ||
            !InBounds(B, ProgramLength) ||
            !InBounds(C, ProgramLength))
        {
            break;
        }

        // The SUBLEQ operation.
        if ((Deref(B) = Deref(A) - Deref(B)) <= 0)
            ProgramCounter = C;

        printf("%d\n", Deref(B));
    }
    while (InBounds(ProgramCounter, ProgramLength) &&
           !IsStdout(ProgramCounter));


    if (!InBounds(ProgramCounter, ProgramLength))
    {
        printf("Program counter is out-of-bounds, exiting.\n");
        return OFFSET_OUT_OF_BOUNDS;
    }
    else if (!InBounds(A, ProgramLength) ||
             !InBounds(B, ProgramLength) ||
             !InBounds(C, ProgramLength))
    {
        printf("Attempted to access an out-of-bounds offset, exiting.\n");
        return OFFSET_OUT_OF_BOUNDS;
    }
    else
    {
        return NORMAL;
    }
}
