/**
 * @file lexer.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-09-01
 *
 * This file contains all the definitions for the funcionality of the SUBLEQC
 * lexer.
 */

#pragma once


/* C Standard Libraries. */
#include <algorithm>

/* Own Libraries. */
#include "token.cpp"
#include "stack.cpp"


/* Structs */

struct lexer {
    char *Source;
    unsigned int Cursor;
    unsigned int LineNumber;
    unsigned int ColumnNumber;
};


/* Functions */

/**
 * Peeks at the character currently under the [Lexer]'s cursor.
 */
static inline
char PeekCursor(lexer *Lexer)
{
    return Lexer->Source[Lexer->Cursor];
}

/**
 * Peeks at the character immediately following the [Lexer]'s cursor.
 */
static inline
char PeekNextCursor(lexer *Lexer)
{
    // If we are at the end of the source, don't read past it.
    if (PeekCursor(Lexer) == '\0')
        return '\0';

    else
        return Lexer->Source[Lexer->Cursor + 1];
}

/**
 * Advances the [Lexer]'s cursor a single character and increments the
 * [Lexer]'s column number, returning the character under the moved cursor. It
 * additionaly checks the character it's advancing the cursor from. If the
 * character is a newline, it increments the [Lexer]'s line number and resets
 * the column number to zero. If the character is a null terminator, it doesn't
 * advance the cursor.
 */
static inline // NOTE[joe] Maybe not inline?
char AdvanceCursor(lexer *Lexer)
{
    char CurrentCharacter = PeekCursor(Lexer);

    if (CurrentCharacter == '\n')
    {
        ++Lexer->LineNumber;
        Lexer->ColumnNumber = 0;
    }

    if (CurrentCharacter != '\0')
    {
        ++Lexer->Cursor;
        ++Lexer->ColumnNumber;
    }

    return PeekCursor(Lexer);
}

/**
 * Checks if the given [Character] is classified as ignorable whitespace.
 */
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

/**
 * Checks if the given [Character] is classified as a digit.
 */
static
bool IsDigit(const char Character)
{
    int DecimalValue = (int) Character;
    return DecimalValue >= 48 && DecimalValue <= 57;
}

/**
 * Checks if the given [Character] is classified as a sign.
 */
static
bool IsSign(const char Character)
{
    return Character == '-' || Character == '+';
}

/**
 * Checks if the given [Text] classifies as a number.
 */
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

/**
 * Checks if the given [Text] classifies as a question mark.
 */
static
bool IsQuestionMark(const char* Text, unsigned int TextLength)
{
    return TextLength == 1 && Text[0] == '?';
}

/**
 * Checks if the given [Text] classifies as a comma.
 */
static
bool IsComma(const char* Text, unsigned int TextLength)
{
    return TextLength == 1 && Text[0] == ',';
}

/**
 * Checks if the given [Text] classifies as an end-of-line (EOL).
 */
static
bool IsEOL(const char* Text, unsigned int TextLength)
{
    return TextLength == 1 &&
           (Text[0] == '\n' || Text[0] == ';');
}

static
bool IsIdentifier(const char* Text, unsigned int TextLength)
{
    for (unsigned int i = 0; i < TextLength; i++)
    {
        if (Text[i] == ',' ||
            Text[i] == ':' ||
            IsEOL(&Text[i], 1) ||
            IsWhitespace(Text[i]))
        {
            return false;
        }
    }

    return true;
}

static
bool IsLabel(const char* Text, unsigned int TextLength)
{
    return IsIdentifier(Text, TextLength - 1) &&
           Text[TextLength - 1] == ':';
}

/**
 * Checks if the given [Text] matches any of the known token types. If it does
 * not, the type is reported to be INVALID.
 */
static inline
token_type CheckTokenType(const char *Text, unsigned int TextLength)
{
#define Match(F,T)  if (Is##F(Text, TextLength)) return T;

    Match(Number, NUMBER)
    else Match(QuestionMark, QMARK)
    else Match(Comma, COMMA)
    else Match(EOL, EOL)
    else Match(Identifier, IDENT)
    else Match(Label, LABEL)
    else
    {
        return INVALID;
    }
}

/**
 * Uses the given [Lexer] to produce a token.
 */
static
token NextToken(lexer *Lexer)
{
    token       Token = { };
    stack<char> Stack = { };

    token_type CurrentType = NONE;
    token_type    NextType = NONE;


    for (char Character = PeekCursor(Lexer);
         Character != '\0';
         Character = AdvanceCursor(Lexer))
    {
        if (IsWhitespace(Character))
            continue;


        Push<char>(&Stack, Character);
        CurrentType = CheckTokenType(Stack.Data, Stack.Length);

        char NextCharacter = PeekNextCursor(Lexer);

        Push<char>(&Stack, NextCharacter);
        NextType = CheckTokenType(Stack.Data, Stack.Length);

        Pop<char>(&Stack);


        if (NextType == INVALID &&
            (CurrentType != INVALID || CheckTokenType(&NextCharacter, 1) != INVALID))
        {
            std::copy(Stack.Data,
                      Stack.Data + Stack.Length,
                      Token.Text);

            // note(jrm): This will cause segfault if the stack is too long!
            // If the stack length is 32, then we will write to the 32'nd index,
            // which will be out of bounds for the token text field.
            // We don't have to worry about anything longer, since that segfault
            // will happen during the copy executed above.
            Token.Text[Stack.Length] = '\0';

            Token.Type = CurrentType;
            Token.LineNumber = Lexer->LineNumber;
            Token.LastColumn = Lexer->ColumnNumber;

            // NOTE[joe] Empty frees any memory we allocated to [Stack].
            // To prevent the memory being leaked, we'll use Empty to clean up.
            Empty<char>(&Stack);

            // NOTE[joe] Since we are exiting immediately after this, we need
            // to advance the cursor forward, otherwise we'll get stuck in an
            // infinite loop.
            //
            // Speaking from experience on this one...
            AdvanceCursor(Lexer);

            return Token;
        }
    }


    return Token;
}
