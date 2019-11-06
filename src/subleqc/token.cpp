/**
 * @file token.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-11
 *
 * This file contains all our declarations and functionality regarding tokens.
 */

#pragma once


/* C Standard Libraries */
#include <algorithm>

/* Own Libraries */
#include "util.cpp"


/* Structs */

enum token_type {
    NONE,
    NUMBER,
    QMARK,
    COMMA,
    EOL,
    IDENT,
    LABEL,
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
    char* Return = new char[16];
    std::fill(Return, Return + 16, '\0');

#define CASE(T, S) case T: \
                   { \
                       std::string String(S); \
                       std::copy(String.begin(), String.end(), Return); \
                   } break;

    switch (Type)
    {
        CASE(NUMBER, "number")
        CASE(QMARK, "question mark")
        CASE(COMMA, "comma")
        CASE(EOL, "end of line")
        CASE(IDENT, "identifier")
        CASE(LABEL, "label")
        CASE(INVALID, "invalid token")

        default:
        {
            Unreachable();
        }
    }

    return Return;
}
