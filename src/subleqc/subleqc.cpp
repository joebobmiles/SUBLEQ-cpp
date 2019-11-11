/**
 * @file subleqc.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-06-10
 *
 * This is the assembler for turning SUBLEQ assembly into a plain binary that
 * is to be interpreted by subleq.exe.
 *
 * TODO[joe]:
 * - Add commandline argument parsing.
 */

// C/C++ stdlib
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <algorithm>

// Internal libs
#include "util.cpp"
#include "buffer.cpp"
#include "stack.cpp"
#include "token.cpp"
#include "lexer.cpp"
#include "hashmap.cpp"


#define UsageString "Usage: subleqc <input file> <output file>\n"


enum exit_status {
    NORMAL,
    MISSING_ARGS,
    SYNTAX_ERROR,
    UNKNOWN
};

struct instruction {
    int Parameters[3];
    unsigned int ParameterCount;
    unsigned int Location;
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

    // MAGIC[joe] 256 is an arbitray value selected based on an estimation of
    // how long an error message is expected to be. This has no other basis
    // than some sloppy mental math.
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

        return MISSING_ARGS;
    }
    else if (argc == 2)
    {
        Severe("Missing either input or output file name, exiting.\n");

        printf(UsageString);

        return MISSING_ARGS;
    }

    std::ifstream SourceFile (argv[1],
                              std::ifstream::in | std::ifstream::binary);

    if (!SourceFile)
    {
        Error("Failed to open input file \"%s\", exiting.\n", argv[1]);

        return UNKNOWN;
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

    buffer<instruction> Instructions = { };
    instruction         CurrentInstruction = { };

    buffer<error> Errors = { };

    hashmap Labels = { };

#define PARSER_STATES \
    _(INVALID) \
    _(START) \
    _(LABEL) \
    _(PARAM) \
    _(COMMA) \

    enum parser_state {
#define _(S) PARSER_STATE_##S,
        PARSER_STATES
#undef _
    } ParserState = PARSER_STATE_START;


    for (token Token = NextToken(&Lexer);
         Token.Type != NONE;
         Token = NextToken(&Lexer))
    {

#define TokenIs(T) if (Token.Type == T)
#define StateIs(S) if (ParserState == PARSER_STATE_##S)

#define AddParameter(V) CurrentInstruction.Parameters[CurrentInstruction.ParameterCount++] = V
#define TransitionTo(S) ParserState = PARSER_STATE_##S

        // If the token we're reading is a NUMBER and the state we're at is...
        TokenIs(NUMBER)
        {
            StateIs(START)
            {
                CurrentAddress++;
                AddParameter(atoi(Token.Text));

                TransitionTo(PARAM);
            }
            else StateIs(LABEL)
            {
                CurrentAddress++;
                AddParameter(atoi(Token.Text));

                TransitionTo(PARAM);
            }
            else StateIs(COMMA)
            {
                CurrentAddress++;
                AddParameter(atoi(Token.Text));

                TransitionTo(PARAM);
            }
            else
            {
                // TODO[joe] Error reporting...
                Unreachable();
            }
        }
        // If the token we're reading is a QMARK and the state we're at is...
        else TokenIs(QMARK)
        {
            StateIs(START)
            {
                AddParameter(++CurrentAddress);

                TransitionTo(PARAM);
            }
            else StateIs(LABEL)
            {
                AddParameter(++CurrentAddress);

                TransitionTo(PARAM);
            }
            else StateIs(COMMA)
            {
                AddParameter(++CurrentAddress);

                TransitionTo(PARAM);
            }
            else
            {
                // TODO[joe] Error reporting...
                Unreachable();
            }
        }
        // If the token we're reading is an IDENT and the state we're at is...
        else TokenIs(IDENT)
        {
            StateIs(START)
            {
                status<unsigned int> Result = Get(&Labels, Token.Text);

                if (Result.Status == ERROR)
                {
                    Error("Undeclared identifier: %s\n", Token.Text);
                    // todo(jrm): Determine if all we need to do is report an
                    // error, or add it to the error queue.
                }
                else
                {
                    unsigned int Address = Result.unpack();

                    // todo(jrm): Fix AddParameter() macro so that it takes
                    // CurrentAddress as a parameter. Right now it's effectively
                    // a global variable, which makes it hard to understand why
                    // we increment CurrentAddress _before_ performing
                    // AddParameter(), despite it being _VERY_ important that
                    // you do so.
                    CurrentAddress++;
                    AddParameter(Address);
                }

                TransitionTo(PARAM);
            }
            else StateIs(LABEL)
            {
                status<unsigned int> Result = Get(&Labels, Token.Text);

                if (Result.Status == ERROR)
                {
                    Error("Undeclared identifier: %s\n", Token.Text);
                    // todo(jrm): Determine if all we need to do is report an
                    // error, or add it to the error queue.
                }
                else
                {
                    unsigned int Address = Result.unpack();

                    // todo(jrm): Fix AddParameter() macro so that it takes
                    // CurrentAddress as a parameter. Right now it's effectively
                    // a global variable, which makes it hard to understand why
                    // we increment CurrentAddress _before_ performing
                    // AddParameter(), despite it being _VERY_ important that
                    // you do so.
                    CurrentAddress++;
                    AddParameter(Address);
                }

                TransitionTo(PARAM);
            }
            else StateIs(COMMA)
            {
                status<unsigned int> Result = Get(&Labels, Token.Text);

                if (Result.Status == ERROR)
                {
                    Error("Undeclared identifier \"%s\"\n", Token.Text);
                    // todo(jrm): Determine if all we need to do is report an
                    // error, or add it to the error queue.
                }
                else
                {
                    unsigned int Address = Result.unpack();

                    // todo(jrm): Fix AddParameter() macro so that it takes
                    // CurrentAddress as a parameter. Right now it's effectively
                    // a global variable, which makes it hard to understand why
                    // we increment CurrentAddress _before_ performing
                    // AddParameter(), despite it being _VERY_ important that
                    // you do so.
                    CurrentAddress++;
                    AddParameter(Address);
                }

                TransitionTo(PARAM);
            }
            else
            {
                // TODO[joe] Error reporting...
                Unreachable();
            }
        }
        // If the token we're reading is a LABEL and the state we're at is...
        else TokenIs(LABEL)
        {
            StateIs(START)
            {
                unsigned int LabelLength = std::strlen(Token.Text) - 1;

                char *Label = new char[LabelLength];
                std::fill(Label, Label+LabelLength, '\0');
                std::copy(Token.Text, Token.Text+LabelLength, Label);

                Put(&Labels, Label, CurrentAddress);

                TransitionTo(LABEL);
            }
            else StateIs(COMMA)
            {
                unsigned int LabelLength = std::strlen(Token.Text) - 1;

                char *Label = new char[LabelLength];
                std::fill(Label, Label+LabelLength, '\0');
                std::copy(Token.Text, Token.Text+LabelLength, Label);

                Put(&Labels, Label, CurrentAddress);

                TransitionTo(LABEL);
            }
            else
            {
                // TODO[joe] Error reporting...
                Unreachable();
            }
        }
        // If the token we're reading is a COMMA and the state we're at is...
        else TokenIs(COMMA)
        {
            StateIs(PARAM)
            {
                // NOTE[joe] Can we remove COMMA and instead use START?
                TransitionTo(COMMA);
            }
            else
            {
                // TODO[joe] Error reporting...
                Unreachable();
            }
        }
        // If the token we're reading is an EOL and the state we're at is...
        else TokenIs(EOL)
        {
            StateIs(PARAM)
            {
                // TODO[joe] Close out instruction and reset.
                Append<instruction>(&Instructions, CurrentInstruction);
                CurrentInstruction = { };

                TransitionTo(START);
            }
            else
            {
                // TODO[joe] Error reporting...
                Unreachable();
            }
        }
        // If the token we're reading is an INVALID token, report that we found
        // an error.
        else TokenIs(INVALID)
        {
            // TODO[joe] Error reporting...
            Unreachable();
        }
        // If we reach here, there is definitely a problem...
        else
        {
            Unreachable();
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

            // TODO[joe] Replace this with something that is more C++ idiomatic.
            // This is an esoteric part of the printf() formatting language
            // that I stumbled across looking for a way to do string padding.
            // The details can be found here:
            // https://stackoverflow.com/a/9741091/6785489
            fprintf(stderr, "\t%*.*s^\n\n",
                            Errors[i].FirstColumn,
                            Errors[i].FirstColumn,
                            " ");
        }

        return SYNTAX_ERROR;
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
        return UNKNOWN;
    }

    BinaryFile.write((char *) Program.Data,
                     sizeof(int) * Program.Length);

    BinaryFile.close();


    return NORMAL;
}
