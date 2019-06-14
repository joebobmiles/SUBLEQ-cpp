/**
 * @file subleqc.exe
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-06-10
 *
 * This is the compiler(?) for turning SUBLEQ assembly into a plain binary that
 * is to be interpreted by subleq.exe.
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <fstream>

template <typename T>
struct buffer {
    T *Data;
    unsigned int Length;
    unsigned int _Size;
};

template <typename T>
static
void Append(buffer<T> *Buffer, const T Value)
{
    if (Buffer->_Size == 0)
    {
        Buffer->_Size  = 1;
        Buffer->Length = 0;
        Buffer->Data = (T *)malloc(sizeof(T) * Buffer->_Size);
    }
    else if (Buffer->Length == Buffer->_Size)
    {
        Buffer->_Size *= 2;
        Buffer->Data = (T *)realloc(Buffer->Data,
                                    sizeof(T) * (Buffer->_Size + 1));
    }

    Buffer->Data[Buffer->Length++] = Value;
    Buffer->Data[Buffer->Length] = (T) 0;
}


int main(int argc, char** argv)
{
    // RANT[joe] I severely disapprove of having to stoop to C++ stdlib. But if
    // clang stops yelling at me, I guess I'll just have to suck it up.
    std::ifstream SourceFile (argv[1]);
    assert(SourceFile);

    SourceFile.seekg(0, SourceFile.end);
    long SourceFileSize = SourceFile.tellg();
    SourceFile.seekg(0, SourceFile.beg);

    char *RawProgram = new char[SourceFileSize + 1];

    SourceFile.read(RawProgram, SourceFileSize);

    SourceFile.close();

    // Ensure that our raw program input is a well-formed c-string.
    RawProgram[SourceFileSize + 1] = '\0';


    // NOTE[joe] These can be replaced with std::vectors.
    // But I don't want too because I'm a stubborn bastard.
    buffer<char> Buffer = { };
    buffer<int> Program = { };

    char Current = '\0';
    while ((Current = *RawProgram++) != '\0')
    {
        if (Current == ' ' || Current == '\n')
        {
            Append<int>(&Program, atoi(Buffer.Data));

            Buffer = { };
        }
        else
        {
            Append<char>(&Buffer, Current);
        }
    }

    std::ofstream BinaryFile (argv[2],
                              std::ifstream::in | std::ifstream::binary);
    assert(BinaryFile);

    // RANT[joe] Unlike using fwrite, std::fstream::write() disallows writing
    // arbitrary data to a file. It _must_ be characters. This is a nit-pick,
    // but an unnecissary annoyance none-the-less.
    BinaryFile.write((char *) Program.Data,
                     sizeof(int) * Program.Length);

    BinaryFile.close();

    return 0;
}
