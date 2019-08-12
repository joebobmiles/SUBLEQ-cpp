/**
 * @file token.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-11
 *
 * This file contains all our declarations and functionality regarding tokens.
 */

#pragma once

enum token_type {
    NONE,
    NUMBER,
    QMARK,
    COMMA,
    EOL
};

struct token {
    token_type Type;
    // To quote a friend, 32B should be more than enough for anyone.
    char       Text[32];
};
