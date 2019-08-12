/**
 * @file subleqc.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-06-10
 *
 * This is the assembler for turning SUBLEQ assembly into a plain binary that
 * is to be interpreted by subleq.exe.
 *
 * TODO[joe]:
 * - Create a way to keep track of where errors are in the original input text.
 *   This is so that we can do helpful error messages, such as directing the
 *   user to where their error is in their code.
 * - Create a stream object for tokens. I think this will save us some memory
 *   during execution when it comes to parsing. (By removing an array of tokens
 *   whose used memory is _at least_ the number of tokens stored.)
 * - Separate totkenization and parsing into different operations. This can be
 *   pushed into the indefinite future, since I'm not terribly concerned with
 *   doing these in line with the rest of the main procedure.
 */

// C/C++ stdlib
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <fstream>

// Internal libs
#include "buffer.cpp"
#include "token.cpp"


#define UsageString "Usage: subleqc <input file> <output file>\n"


enum status {
    STATUS_NORMAL,
    STATUS_MISSING_ARGS,
    STATUS_SYNTAX_ERROR,
    STATUS_UNKNOWN
};

struct instruction {
    int Parameters[3];
    unsigned int ParameterCount;
    unsigned int Location;
};


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
    if (argc == 1)
    {
        printf(UsageString
               "Error: No input file specified, exiting.\n");
        return STATUS_MISSING_ARGS;
    }
    else if (argc == 2)
    {
        printf(UsageString
               "Error: Missing either input or output file name, exiting.\n");
        return STATUS_MISSING_ARGS;
    }

    std::ifstream SourceFile (argv[1],
                              std::ifstream::in | std::ifstream::binary);

    if (!SourceFile)
    {
        printf("Error: Failed to open input file \"%s\", exiting.\n", argv[1]);
        return STATUS_UNKNOWN;
    }

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

    /** TODO[joe] I itch to make a macro for these repeated segments of code.
     * Though something tells me that a) a macro for this will be messy as all
     * get-out, and b) there's a way to consolodate this that doesn't need
     * macros. */
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

            CurrentToken.Type = NUMBER;

            Append<token>(&Tokens, CurrentToken);

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

            CurrentToken.Type = QMARK;

            Append<token>(&Tokens, CurrentToken);

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

            CurrentToken.Type = COMMA;

            Append<token>(&Tokens, CurrentToken);

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

            CurrentToken.Type = EOL;

            Append<token>(&Tokens, CurrentToken);

            LastCursor = CurrentCursor;

            WasEOL = false;
        }

        // Skip whitespace
        else if (IsWhitespace(*LastCursor)) LastCursor = ++CurrentCursor;

        // TODO[joe] Report error and halt when we encounter something we don't
        // recognize. (How do we report the location of the unrecognized
        // symbol?)
        else
        {
            printf("Encountered unrecognized symbol \'%c\'!\n", *LastCursor);
        }

    } while (*CurrentCursor != '\0');

    Append<token>(&Tokens, { .Type = EOL });


    unsigned int CurrentAddress = 0;

    buffer<instruction> Instructions = { };
    instruction         CurrentInstruction = { };

    for (unsigned int i = 0; i < Tokens.Length; i++)
    {
        token Token = Tokens.Data[i];

        switch (Token.Type)
        {
            case NUMBER:
            {
                if (CurrentInstruction.ParameterCount == 3)
                {
                    // TODO[joe] A more useful error message.
                    printf("Error: An instruction can have only 3 parameters.\n");

                    return STATUS_SYNTAX_ERROR;
                }

                else
                {
                    unsigned int ParameterCount =
                                            CurrentInstruction.ParameterCount++;

                    CurrentInstruction.Parameters[ParameterCount] =
                                                            atoi(Token.Text);
                }

                CurrentAddress++;
            } break;

            case QMARK:
            {
                if (CurrentInstruction.ParameterCount == 3)
                {
                    // TODO[joe] A more useful error message.
                    printf("Error: An instruction can have only 3 parameters.\n");

                    return STATUS_SYNTAX_ERROR;
                }

                else
                {
                    unsigned int ParameterCount =
                                            CurrentInstruction.ParameterCount++;

                    CurrentInstruction.Parameters[ParameterCount] =
                                                            ++CurrentAddress;
                }
            } break;

            case EOL:
            {
                Append<instruction>(&Instructions, CurrentInstruction);

                CurrentInstruction = { .Location = CurrentAddress };
            } break;

            // We don't care about commas.
            case COMMA: default:
                break;
        }
    }


    buffer<int> Program = { };

    for (unsigned int i = 0; i < Instructions.Length; i++)
    {
        instruction Instruction = Instructions.Data[i];

        switch (Instruction.ParameterCount)
        {
            case 1:
            {
                Append<int>(&Program, Instruction.Parameters[0]);
                Append<int>(&Program, Instruction.Parameters[0]);
                Append<int>(&Program, Instruction.Location + 3);
            } break;

            case 2:
            {
                Append<int>(&Program, Instruction.Parameters[0]);
                Append<int>(&Program, Instruction.Parameters[1]);
                Append<int>(&Program, Instruction.Location + 3);
            } break;

            case 3:
            {
                Append<int>(&Program, Instruction.Parameters[0]);
                Append<int>(&Program, Instruction.Parameters[1]);
                Append<int>(&Program, Instruction.Parameters[2]);
            } break;

            // TODO[joe] Raise error? This is a case we should never reach.
            default:
                break;
        }
    }


    std::ofstream BinaryFile (argv[2],
                              std::ofstream::out | std::ofstream::binary);

    if (!BinaryFile)
    {
        printf("Error: Failed to open output file \"%s\", exiting.\n", argv[2]);
        return STATUS_UNKNOWN;
    }

    BinaryFile.write((char *) Program.Data,
                     sizeof(int) * Program.Length);

    BinaryFile.close();


    return STATUS_NORMAL;
}
