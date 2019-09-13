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
 * - Add commandline argument parsing.
 */

// C/C++ stdlib
#include <cstdio>
#include <cassert>
#include <fstream>

// Internal libs
#include "util.cpp"
#include "buffer.cpp"
#include "token.cpp"
#include "lexer.cpp"


#define UsageString "Usage: subleqc <input file> <output file>\n"


enum status {
    STATUS_NORMAL,
    STATUS_MISSING_ARGS,
    STATUS_SYNTAX_ERROR,
    STATUS_UNKNOWN
};

struct error {
    char *Message;
    char *SourceLine;
    unsigned int LineNumber;
    unsigned int FirstColumn;
    unsigned int LastColumn;
};


static
error GenerateError(const char *Message, token *Token)
{
    error Error = { };

    char *TypeString = TokenTypeToString(Token->Type);

    // MAGIC[joe] 256 is an arbitray value selected based
    // on an estimation of how long an error message is
    // expected to be. This has no other basis than some
    // sloppy mental math.
    Error.Message = new char[256];

    sprintf(Error.Message,
            Message,
            TokenTypeToString(Token->Type));

    Error.LineNumber  = Token->LineNumber;
    Error.FirstColumn = Token->LastColumn - strlen(Token->Text);
    Error.LastColumn  = Token->LastColumn;

    return Error;
}


int main(int argc, char** argv)
{
    if (argc == 1)
    {
        Severe("No input file specified, exiting.\n");

        printf(UsageString);

        return STATUS_MISSING_ARGS;
    }
    else if (argc == 2)
    {
        Severe("Missing either input or output file name, exiting.\n");

        printf(UsageString);

        return STATUS_MISSING_ARGS;
    }

    std::ifstream SourceFile (argv[1],
                              std::ifstream::in | std::ifstream::binary);

    if (!SourceFile)
    {
        Error("Failed to open input file \"%s\", exiting.\n", argv[1]);

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


    /** Split source into lines for error reporting. */
    buffer<char *> Lines = { };
    buffer<char>   Line  = { };

    for (char *Character = RawProgram;
         *Character != '\0';
         Character++)
    {
        Append<char>(&Line, *Character);

        if (*Character == '\n')
        {
            Append<char>(&Line, '\0');
            Append<char *>(&Lines, Line.Data);

            // NOTE[joe] We don't use Empty() here since it would free memory
            // we'd want to use later.
            Line = { };
        }
    }


    /** Configure lexer. */

    lexer Lexer  = { };
    Lexer.Source = RawProgram;


    /** Parse tokens into instructions. */

    unsigned int CurrentAddress = 0;

    struct instruction {
        int Parameters[3];
        unsigned int ParameterCount;
        unsigned int Location;
    };

    buffer<instruction> Instructions = { };
    instruction         CurrentInstruction = { };

    enum parser_state {
        PARSER_STATE_NONE,
        PARSER_STATE_START,
        PARSER_STATE_FIRST_PARAM,
        PARSER_STATE_FIRST_COMMA, // TODO[joe] Get rid of commas, logic is simpler.
        PARSER_STATE_SECOND_PARAM,
        PARSER_STATE_SECOND_COMMA, // TODO[joe] Get rid of commas, logic is simpler.
        PARSER_STATE_THIRD_PARAM,
        PARSER_STATE_EOL,
    };

    parser_state ParserState = PARSER_STATE_START;

    buffer<error> Errors = { };


    for (token Token = NextToken(&Lexer);
         Token.Type != NONE;
         Token = NextToken(&Lexer))
    {
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
                        error Error = GenerateError("Unexpected %s when reading"
                                                    " instruction. Expected "
                                                    "number or question mark.",
                                                    &Token);

                        Append<error>(&Errors, Error);
                    } break;
                }
            } break;

            case PARSER_STATE_FIRST_PARAM:
            case PARSER_STATE_SECOND_PARAM:
            {
                switch (Token.Type)
                {
                    case COMMA:
                    {

                        switch (ParserState)
                        {
                            case PARSER_STATE_FIRST_PARAM:
                                ParserState = PARSER_STATE_FIRST_COMMA; break;

                            case PARSER_STATE_SECOND_PARAM:
                                ParserState = PARSER_STATE_SECOND_COMMA; break;

                            default: Unreachable(); break;
                        }
                    } break;

                    // Set the state to PARSER EOL
                    case EOL:
                    {
                        Append<instruction>(&Instructions, CurrentInstruction);

                        ParserState = PARSER_STATE_START;
                    } break;

                    // If we read something we're not supposed to, then we need
                    // to report an error.
                    default:
                    {
                        error Error = GenerateError("Unexpected %s when reading "
                                                    "instruction. Expected comma.",
                                                    &Token);

                        Append<error>(&Errors, Error);
                    } break;
                }
            } break;

            case PARSER_STATE_FIRST_COMMA:
            case PARSER_STATE_SECOND_COMMA:
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
                            case PARSER_STATE_FIRST_COMMA:
                            {
                                ParserState = PARSER_STATE_SECOND_PARAM;
                            } break;

                            case PARSER_STATE_SECOND_COMMA:
                            {
                                ParserState = PARSER_STATE_THIRD_PARAM;
                            } break;

                            default:
                            {
                                Unreachable();
                            } break;
                        }
                    } break;

                    // If we read something we're not supposed to, then we need
                    // to report an error.
                    default:
                    {
                        error Error = GenerateError("Unexpected %s when reading "
                                                    "instruction. Expected "
                                                    "number or question mark.", 
                                                    &Token);

                        Append<error>(&Errors, Error);
                    } break;
                }
            } break;

            case PARSER_STATE_THIRD_PARAM:
            {
                switch (Token.Type)
                {
                    case EOL:
                    {
                        Append<instruction>(&Instructions, CurrentInstruction);

                        ParserState = PARSER_STATE_START;
                    } break;

                    // If we read something we're not supposed to, then we need
                    // to report an error.
                    default:
                    {
                        error Error = GenerateError("Unexpected %s when reading "
                                                    "instruction. Expected end "
                                                    "of line.",
                                                    &Token);

                        Append<error>(&Errors, Error);
                    } break;
                }
            } break;

            default:
            {
                // TODO[joe] Will we ever reach this?
                Unreachable();
            } break;
        }
    }


    if (Errors.Length)
    {
        for (unsigned int i = 0; i < Errors.Length; i++)
        {
            Error("Line %d, column %d: %s\n\n\t%s",
                   Errors[i].LineNumber + 1,
                   Errors[i].FirstColumn + 1,
                   Errors[i].Message,
                   Lines[Errors[i].LineNumber]);

            // This is an esoteric part of the printf() formatting language
            // that I stumbled across looking for a way to do string padding.
            // The details can be found here:
            // https://stackoverflow.com/a/9741091/6785489
            fprintf(stderr, "\t%*.*s^\n\n",
                            Errors[i].FirstColumn,
                            Errors[i].FirstColumn,
                            " ");
        }

        return STATUS_SYNTAX_ERROR;
    }


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
            {
                Unreachable();
            } break;
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
