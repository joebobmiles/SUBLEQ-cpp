/**
 * @file token.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-11
 *
 * This file contains all our declarations and functionality regarding tokens.
 */

#pragma once


/* C Standard Libraries */

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
    char* Return = new char[16];

    switch (Type)
    {
        case NUMBER:
        {
            std::string String("number");
            std::copy(String.begin(), String.end(), Return);
        } break;

        case QMARK:
        {
            std::string String("question mark");
            std::copy(String.begin(), String.end(), Return);
        } break;

        case COMMA:
        {
            std::string String("comma");
            std::copy(String.begin(), String.end(), Return);
        } break;

        case EOL:
        {
            std::string String("end of line");
            std::copy(String.begin(), String.end(), Return);
        } break;

        case INVALID:
        default:
        {
            std::string String("invalid token");
            std::copy(String.begin(), String.end(), Return);
        } break;
    }

    return Return;
}
