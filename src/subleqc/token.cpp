/**
 * @file token.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-11
 *
 * This file contains all our declarations and functionality regarding tokens.
 */

#pragma once


/* C Standard Libraries */
#include <cstdio>
#include <cstdlib>

/* Own Libraries */


/* Structs */

enum token_type {
    NONE,
    NUMBER,
    QMARK,
    COMMA,
    EOL,
    INVALID,
};


/* Functions */

struct token {
    token_type Type;
    // To quote a friend, 32B should be more than enough for anyone.
    char Text[32];
    unsigned int LineNumber;
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
