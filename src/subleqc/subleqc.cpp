/**
 * @file subleqc.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-06-10
 *
 * This is the assembler for turning SUBLEQ assembly into a plain binary that
 * is to be interpreted by subleq.exe.
 *
 * TODO[joe]:
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


    /** Split source program flat array into 2D array of lines. */

    buffer<char*>  Lines = { };
    char          *Line  = 0;

    buffer<char> TempLine = { };

    for (char* Cursor = RawProgram;
         *Cursor != '\0';
         Cursor++)
    {
        Append<char>(&TempLine, *Cursor);

        if (*Cursor == '\n')
        {
            Line = (char *)malloc(sizeof(char) * TempLine.Length + 1);

            memcpy(Line, TempLine.Data, TempLine.Length);
            Line[TempLine.Length] = '\0';

            Append<char*>(&Lines, Line);

            TempLine = { };
        }
    }


    /** Tokenize lines. */

    buffer<token> Tokens = { };

    for (unsigned int i = 0; i < Lines.Length; i++)
    {
        buffer<token> TempTokens = { };
        token         CurrentToken = { };

        unsigned int LineNumber = i + 1;

        char* LastCursor = Lines[i];
        char* CurrentCursor = Lines[i];

        bool WasNumber       = false;
        bool WasQuestionMark = false;
        bool WasComma        = false;
        bool WasEOL          = false;

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
                CurrentToken.Line = Lines[i];
                CurrentToken.FirstColumn = LastCursor - Lines[i];
                CurrentToken.LastColumn  = CurrentCursor - Lines[i];

                Append<token>(&TempTokens, CurrentToken);

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
                CurrentToken.Line = Lines[i];
                CurrentToken.FirstColumn = LastCursor - Lines[i];
                CurrentToken.LastColumn  = CurrentCursor - Lines[i];

                Append<token>(&TempTokens, CurrentToken);

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
                CurrentToken.Line = Lines[i];
                CurrentToken.FirstColumn = LastCursor - Lines[i];
                CurrentToken.LastColumn  = CurrentCursor - Lines[i];

                Append<token>(&TempTokens, CurrentToken);

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
                CurrentToken.Line = Lines[i];
                CurrentToken.FirstColumn = LastCursor - Lines[i];
                CurrentToken.LastColumn  = CurrentCursor - Lines[i];

                Append<token>(&TempTokens, CurrentToken);

                LastCursor = CurrentCursor;

                WasEOL = false;
            }

            // Skip whitespace
            else if (IsWhitespace(*LastCursor)) LastCursor = ++CurrentCursor;

            else
            {
                printf("Encountered unrecognized symbol \'%c\' on line %d, col %d:\n",
                        *LastCursor, LineNumber, (int) (LastCursor - Lines[i])+1);

                printf("\n\t%s", Lines[i]);

                // This is an esoteric part of the printf() formatting language
                // that I stumbled across looking for a way to do string padding.
                // The details can be found here:
                // https://stackoverflow.com/a/9741091/6785489
                printf("\t%*.*s^\n", (int) (LastCursor - Lines[i]),
                                     (int) (LastCursor - Lines[i]),
                                     " ");

                // Abort execution.
                // TODO[joe] Should we just accumulate errors and report them
                // all at once instead of aborting on the first error
                // encountered?
                return STATUS_SYNTAX_ERROR;
            }

        } while (*CurrentCursor != '\0');

        Append<token>(&TempTokens, { .Type = EOL });

        for (unsigned int j = 0; j < TempTokens.Length; j++)
        {
            Append<token>(&Tokens, TempTokens[j]);
        }
    }


    /** Parse tokens into instructions. */

    unsigned int CurrentAddress = 0;

    buffer<instruction> Instructions = { };
    instruction         CurrentInstruction = { };

    // TODO[joe] Convert to proper automata instead of doing this some-what
    // hacky business.

    enum parser_state {
        PARSER_STATE_START,
        PARSER_STATE_FIRST_PARAM,
        PARSER_STATE_SECOND_PARAM,
        PARSER_STATE_THIRD_PARAM,
        PARSER_STATE_EOL,
    } ParserState = PARSER_STATE_START;

    struct error {
        char *Message;
        char *SourceLine;
        unsigned int LineNumber;
        unsigned int FirstColumn;
        unsigned int LastColumn;
    };

    buffer<error> Errors = { };

    for (unsigned int i = 0; i < Tokens.Length; i++)
    {
        token Token = Tokens[i];

        switch (ParserState)
        {
            case PARSER_STATE_START:
            {
                switch (Token.Type)
                {
                    case NUMBER:
                    case QMARK:
                    {
                        CurrentInstruction = { };

                        int Index = CurrentInstruction.ParameterCount++;

                        if (Token.Type == QMARK)
                        {
                            CurrentInstruction.Parameters[Index] = ++CurrentAddress;
                        }
                        else
                        {
                            ++CurrentAddress;
                            CurrentInstruction.Parameters[Index] = atoi(Token.Text);
                        }

                        ParserState = PARSER_STATE_FIRST_PARAM;
                    } break;

                    case EOL:
                    break;

                    // If we read something we're not supposed to, then we need
                    // to report an error.
                    default:
                    {
                        error Error = { };


                        char *TypeString = TokenTypeToString(Token.Type);

                        // MAGIC[joe] 256 is an arbitray value selected based
                        // on an estimation of how long an error message is
                        // expected to be. This has no other basis than some
                        // sloppy calculations.
                        Error.Message = (char *)malloc(sizeof(char) * 256);

                        sprintf(Error.Message,
                                "Unexpected %s when reading instruction. "
                                "Expected number or question mark.",
                                TokenTypeToString(Token.Type));


                        Error.SourceLine = (char *)malloc(sizeof(char) *
                                                          strlen(Token.Line));

                        sprintf(Error.SourceLine, "%s", Token.Line);


                        Error.LineNumber  = Token.LineNumber;
                        Error.FirstColumn = Token.FirstColumn;
                        Error.LastColumn  = Token.LastColumn;


                        Append<error>(&Errors, Error);
                    } break;
                }
            } break;

            case PARSER_STATE_FIRST_PARAM:
            case PARSER_STATE_SECOND_PARAM:
            {
                switch (Token.Type)
                {
                    case NUMBER:
                    case QMARK:
                    {
                        int Index = CurrentInstruction.ParameterCount++;

                        if (Token.Type == QMARK)
                        {
                            CurrentInstruction.Parameters[Index] = ++CurrentAddress;
                        }
                        else
                        {
                            ++CurrentAddress;
                            CurrentInstruction.Parameters[Index] = atoi(Token.Text);
                        }

                        switch (ParserState)
                        {
                            case PARSER_STATE_FIRST_PARAM:
                                ParserState = PARSER_STATE_SECOND_PARAM; break;

                            case PARSER_STATE_SECOND_PARAM:
                                ParserState = PARSER_STATE_THIRD_PARAM; break;

                            default: break;
                        }
                    } break;

                    // Set the state to PARSER EOL
                    case EOL:
                    {
                        ParserState = PARSER_STATE_EOL;
                    } break;

                    // If we read something we're not supposed to, then we need
                    // to report an error.
                    default:
                    {
                        error Error = { };


                        char *TypeString = TokenTypeToString(Token.Type);

                        Error.Message = (char *)malloc(sizeof(char) *
                                                       strlen(TypeString));
                        // MAGIC[joe] 256 is an arbitray value selected based
                        // on an estimation of how long an error message is
                        // expected to be. This has no other basis than some
                        // sloppy calculations.
                        Error.Message = (char *)malloc(sizeof(char) * 256);

                        sprintf(Error.Message,
                                "Unexpected %s when reading instruction. "
                                "Expected number or question mark.",
                                TokenTypeToString(Token.Type));


                        Error.SourceLine = (char *)malloc(sizeof(char) *
                                                          strlen(Token.Line));

                        sprintf(Error.SourceLine, "%s", Token.Line);


                        Error.LineNumber  = Token.LineNumber;
                        Error.FirstColumn = Token.FirstColumn;
                        Error.LastColumn  = Token.LastColumn;


                        Append<error>(&Errors, Error);
                    } break;
                }
            } break;

            case PARSER_STATE_THIRD_PARAM:
            {
                switch (Token.Type)
                {
                    // Set the state to PARSER EOL
                    case EOL:
                    {
                        ParserState = PARSER_STATE_EOL;
                    } break;

                    // If we read something we're not supposed to, then we need
                    // to report an error.
                    default:
                    {
                        error Error = { };


                        char *TypeString = TokenTypeToString(Token.Type);

                        // MAGIC[joe] 256 is an arbitray value selected based
                        // on an estimation of how long an error message is
                        // expected to be. This has no other basis than some
                        // sloppy calculations.
                        Error.Message = (char *)malloc(sizeof(char) * 256);

                        sprintf(Error.Message,
                                "Unexpected %s when reading instruction. "
                                "Expected end of line.",
                                TokenTypeToString(Token.Type));


                        Error.SourceLine = (char *)malloc(sizeof(char) *
                                                          strlen(Token.Line));

                        sprintf(Error.SourceLine, "%s", Token.Line);


                        Error.LineNumber  = Token.LineNumber;
                        Error.FirstColumn = Token.FirstColumn;
                        Error.LastColumn  = Token.LastColumn;


                        Append<error>(&Errors, Error);
                    } break;
                }
            } break;

            case PARSER_STATE_EOL:
            {
                Append<instruction>(&Instructions, CurrentInstruction);
            } break;

            // TODO[joe] Will we ever reach this?
            default: break;
        }
    }

    if (Errors.Length)
    {
        for (unsigned int i = 0; i < Errors.Length; i++)
        {
            printf("Error at %d,%d: %s\n\n\t%s",
                   Errors[i].LineNumber,
                   Errors[i].FirstColumn,
                   Errors[i].Message,
                   Errors[i].SourceLine);

            // This is an esoteric part of the printf() formatting language
            // that I stumbled across looking for a way to do string padding.
            // The details can be found here:
            // https://stackoverflow.com/a/9741091/6785489
            printf("\t%*.*s^\n\n",
                   Errors[i].FirstColumn,
                   Errors[i].FirstColumn,
                   " ");
        }

        return STATUS_SYNTAX_ERROR;
    }

    /*
    for (unsigned int i = 0; i < Tokens.Length; i++)
    {
        token Token = Tokens[i];

        switch (Token.Type)
        {
            case NUMBER:
            case QMARK:
            {
                if (CurrentInstruction.ParameterCount == 3)
                {
                    // TODO[joe] A more useful error message.
                    // It might be better to permit more than one parameter to
                    // be added here and then perform a sanity check later on.
                    printf("Error: An instruction can have only 3 parameters.");

                    return STATUS_SYNTAX_ERROR;
                }

                else
                {
                    unsigned int ParameterCount = CurrentInstruction.ParameterCount++;

                    if (Token.Type == QMARK)
                        CurrentInstruction.Parameters[ParameterCount] = ++CurrentAddress;

                    else
                    {
                        CurrentInstruction.Parameters[ParameterCount] = atoi(Token.Text);
                        ++CurrentAddress;
                    }
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
    */


    /** Generate program code. */

    buffer<int> Program = { };

    for (unsigned int i = 0; i < Instructions.Length; i++)
    {
        instruction Instruction = Instructions[i];

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


    /** Output program code to provided binary file. */

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
