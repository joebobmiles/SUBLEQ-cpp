/**
 * @file token.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-11
 *
 * This file contains all our declarations and functionality regarding tokens.
 */

#pragma once


// C Standard Libraries
#include <cstdio>
#include <cstring>

// Own Libraries
#include "buffer.cpp"
#include "stack.cpp"


enum token_type {
    NONE,
    NUMBER,
    QMARK,
    COMMA,
    EOL,
    INVALID,
};

struct token {
    token_type Type;
    // To quote a friend, 32B should be more than enough for anyone.
    char  Text[32];
    char* Line;
    unsigned int LineNumber;
    unsigned int FirstColumn;
    unsigned int LastColumn;
};

static inline
char *TokenTypeToString(token_type Type)
{
    char* Return = (char *)malloc(sizeof(char) * 16);

    switch (Type)
    {
        case NUMBER:
        {
            sprintf(Return, "number");
        } break;

        case QMARK:
        {
            sprintf(Return, "question mark");
        } break;

        case COMMA:
        {
            sprintf(Return, "comma");
        } break;

        case EOL:
        {
            sprintf(Return, "end of line");
        } break;

        case INVALID:
        default:
        {
            sprintf(Return, "invalid");
        } break;
    }

    return Return;
}


struct tokenizer {
    char *Text;
    unsigned int Cursor;
    unsigned int Line;
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

static inline
token_type CheckTokenType(const char *Text, unsigned int TextLength)
{
#define Match(F,T)  if (Is##F(Text, TextLength)) return T;

    Match(Number, NUMBER)
    else Match(QuestionMark, QMARK)
    else Match(Comma, COMMA)
    else Match(EOL, EOL)
    else
    {
        return INVALID;
    }
}

static inline
void SkipWhitespace(tokenizer *Tokenizer)
{
    while (IsWhitespace(Tokenizer->Text[Tokenizer->Cursor]))
        Tokenizer->Cursor++;
}

static
token NextToken(tokenizer *Tokenizer)
{
    stack<char> Stack = { };
    token Token = { };

    SkipWhitespace(Tokenizer);

    char Character = Tokenizer->Text[Tokenizer->Cursor];
    Push<char>(&Stack, Character);

    while (Character != '\0')
    {
        if (IsWhitespace(Character))
        {
            Character = Tokenizer->Text[++Tokenizer->Cursor];
            continue;
        }

        token_type CurrentType = CheckTokenType(Stack.Data, Stack.Length);

        Character = Tokenizer->Text[++Tokenizer->Cursor];
        Push<char>(&Stack, Character);

        token_type NextType = CheckTokenType(Stack.Data, Stack.Length);

        if (CurrentType != NextType)
        {
            Pop<char>(&Stack);

            // TODO[joe] Create a token.
            
            memcpy(Token.Text, Stack.Data, Stack.Length);

            Token.Type = CurrentType;

            return Token;
        }
        else if (CurrentType == NextType && CurrentType == INVALID)
        {
            if (CheckTokenType(&Character, 1) != INVALID)
            {
                Pop<char>(&Stack);

                // TODO[joe] Create INVALID token.
                
                memcpy(Token.Text, Stack.Data, Stack.Length);

                Token.Type = CurrentType;

                return Token;
            }
        }
    }

    return Token;
}
