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
                                    sizeof(T) * Buffer->_Size);
    }

    Buffer->Data[Buffer->Length++] = Value;
    Buffer->Data[Buffer->Length] = (T) 0;
}


int main(int argc, char** argv)
{
    FILE *Source = fopen(argv[1], "r");
    assert(Source != 0);

    fseek(Source, 0, SEEK_END);
    long SourceSize = ftell(Source);
    rewind(Source);

    // NOTE[joe] We may not need to malloc this memory...
    char *RawProgram = (char *)malloc(sizeof(char) * (SourceSize + 1));
    assert(RawProgram != 0);

    fread((void *)RawProgram,
          sizeof(char),
          SourceSize / sizeof(char),
          Source);

    fclose(Source);

    // Ensure that our raw program input is a well-formed c-string.
    RawProgram[SourceSize + 1] = '\0';


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


    FILE *Binary = fopen(argv[2], "w");
    assert(Binary != 0);

    fwrite((void *)Program.Data,
           sizeof(int),
           Program.Length,
           Binary);

    fclose(Binary);

    return 0;
}
