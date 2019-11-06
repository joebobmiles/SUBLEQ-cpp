/**
 * @file util.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-25
 *
 * This file contains some utility macros and functions that help with things
 * like assertions, error reporting, logging, etc...
 */

#pragma once

#include <cstdlib>
#include <cstdio>


/** Error reporting/logging */
#define Info(FORMAT, ...)    fprintf(stdout, "INFO:\t" FORMAT, ##__VA_ARGS__)
#define Warning(FORMAT, ...) fprintf(stdout, "WARN:\t" FORMAT, ##__VA_ARGS__)
#define Error(FORMAT, ...)   fprintf(stderr, "ERROR:\t" FORMAT, ##__VA_ARGS__)
#define Severe(FORMAT, ...)  fprintf(stderr, "SEVERE:\t " FORMAT, ##__VA_ARGS__)


/** Assertions */
#define Assert(EXPRESSION) \
    if (!(EXPRESSION)) { \
        Severe("Assertion \"%s\" failed in %s, line %d", \
               #EXPRESSION, \
               __FILE__, \
               __LINE__); \
        std::abort(); \
    }

#define Unreachable() Severe("Branched into unreachable case in %s, line %d\n", \
                             __FILE__, \
                             __LINE__)


/** Status reporting */

enum status_code {
    OK,
    ERROR
};

template <typename T>
struct status {
    status_code Status;
    T           Value;

    T unpack()
    {
        return this->Value;
    }
};
