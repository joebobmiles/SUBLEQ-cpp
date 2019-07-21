/**
 * @file subleqc.cpp
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

struct token {
    char Text[32];
};

struct instruction {
    int Parameters[3];
    unsigned int ParameterCount;
    unsigned int Location;
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
    Buffer->Data[Buffer->Length] = (T) { };
}

template <typename T>
static inline
void Empty(buffer<T> *Buffer)
{
    Buffer->_Size = 0;
    Buffer->Length = 0;

    free(Buffer->Data);
    Buffer->Data = 0;
}

static
bool IsWhitespace(const char Character)
{
    char WhitespaceSet[] = { ' ', '\t', '\r' };

    for (unsigned int i = 0; i < sizeof(WhitespaceSet); i++)
    {
        if (WhitespaceSet[i] == Character)
            return true;
    }

    return false;
}

static
bool IsDigit(const char Character)
{
    int DecimalValue = (int) Character;
    return DecimalValue >= 48 && DecimalValue <= 57;
}

static
bool IsSign(const char Character)
{
    return Character == '-' || Character == '+';
}

static
bool IsNumber(const char* Text, unsigned int TextLength)
{
    if (!IsSign(Text[0]) && !IsDigit(Text[0]))
        return false;

    for (int i = 1; i < TextLength; i++)
    {
        if (!IsDigit(Text[i]))
            return false;
    }

    return true;
}

static
bool IsQuestionMark(const char* Text, unsigned int TextLength)
{
    return TextLength == 1 && Text[0] == '?';
}

static
bool IsComma(const char* Text, unsigned int TextLength)
{
    return TextLength == 1 && Text[0] == ',';
}

static
bool IsEOL(const char* Text, unsigned int TextLength)
{
    return TextLength == 1 &&
           (Text[0] == '\n' || Text[0] == ';');
}


int main(int argc, char** argv)
{
    std::ifstream SourceFile (argv[1],
                              std::ifstream::in | std::ifstream::binary);
    assert(SourceFile);

    SourceFile.seekg(0, std::ios::end);
    long SourceFileSize = SourceFile.tellg();
    SourceFile.seekg(0, std::ios::beg);

    char *RawProgram = new char[SourceFileSize + 1];

    SourceFile.read(RawProgram, SourceFileSize);

    SourceFile.close();

    // Ensure that our raw program input is a well-formed c-string.
    RawProgram[SourceFileSize] = '\0';


    char* LastCursor = RawProgram;
    char* CurrentCursor = RawProgram;

    bool WasNumber       = false;
    bool WasQuestionMark = false;
    bool WasComma        = false;
    bool WasEOL          = false;

    buffer<token> Tokens = { };
    token         CurrentToken = { };

    /** TODO[joe] I itch to make a macro for these repeated segments of code. */
    do {

        // Match a NUMBER and produce a number token.
        if (IsNumber(LastCursor,
                     (unsigned int) (CurrentCursor - LastCursor + 1)))
        {
            WasNumber = true;

            CurrentCursor++;
        }
        else if (WasNumber)
        {
            memcpy(CurrentToken.Text,
                   LastCursor,
                   (unsigned int) (CurrentCursor - LastCursor));

            printf("<number:%s>", CurrentToken.Text);

            LastCursor = CurrentCursor;

            WasNumber = false;
        }

        // Match a QUESTION MARK and produce a qmark token.
        else if (IsQuestionMark(LastCursor,
                           (unsigned int) (CurrentCursor - LastCursor + 1)))
        {
            WasQuestionMark = true;

            CurrentCursor++;
        }
        else if (WasQuestionMark)
        {
            memcpy(CurrentToken.Text,
                   LastCursor,
                   (unsigned int) (CurrentCursor - LastCursor));

            printf("<qmark>");

            LastCursor = CurrentCursor;

            WasQuestionMark = false;
        }

        // Match a COMMA and produce a comma token.
        else if (IsComma(LastCursor,
                         (unsigned int) (CurrentCursor - LastCursor + 1)))
        {
            WasComma = true;

            CurrentCursor++;
        }
        else if (WasComma)
        {
            memcpy(CurrentToken.Text,
                   LastCursor,
                   (unsigned int) (CurrentCursor - LastCursor));

            printf("<comma>");

            LastCursor = CurrentCursor;

            WasComma = false;
        }

        // Match a SEMICOLON or a NEWLINE and produce a EOL token.
        else if (IsEOL(LastCursor,
                       (unsigned int) (CurrentCursor - LastCursor + 1)))
        {
            WasEOL = true;

            CurrentCursor++;
        }
        else if (WasEOL)
        {
            memcpy(CurrentToken.Text,
                   LastCursor,
                   (unsigned int) (CurrentCursor - LastCursor));

            printf("<eol>\n");

            LastCursor = CurrentCursor;

            WasEOL = false;
        }

        // Skip whitespace
        else if (IsWhitespace(*LastCursor)) LastCursor = ++CurrentCursor;

        // TODO[joe] Report error and halt when we encounter something we don't
        // recognize.
        else printf("[UNKNOWN]");

    } while (*CurrentCursor != '\0');


    /*
    buffer<int> Program = { };

    for (unsigned int i = 0; i < Instructions.Length; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            Append<int>(&Program, Instructions.Data[i].Parameters[j]);
        }
    }


    std::ofstream BinaryFile (argv[2],
                              std::ofstream::out | std::ofstream::binary);
    assert(BinaryFile);

    BinaryFile.write((char *) Program.Data,
                     sizeof(int) * Program.Length);

    BinaryFile.close();
    */

    return 0;
}
