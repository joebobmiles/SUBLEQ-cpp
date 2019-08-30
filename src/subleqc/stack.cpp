/**
 * @file stack.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-30
 *
 * This file contains our implementation of a generic stack.
 */

#pragma once


// C standard libraries
#include <stdlib>

// Own Libraries
#include "util.cpp"


template <typename T>
struct stack
{
    T *Data;
    unsigned int Length;
    unsigned int _Size;
};


template <typename T>
static
void Push(stack<T> *Stack, T Value)
{
    if (Stack->_Size == 0)
    {
        Stack->Data = (T *)malloc(sizeof(T));
        Stack->_Size = 1;
    }
    else if (Stack->Length == Stack->_Size)
    {
        Stack->_Size *= 2;
        Stack->Data = (T *)realloc(sizeof(T) * Stack->_Size);
    }

    Stack->Data[Stack->Length++] = Value;
}

template <typename T>
static inline
T Pop(stack<T> *Stack)
{
    // NOTE[joe] We can't pop from an empty stack, so we'll assert here.
    // Unfortunately our assertions don't cause the program to crash (we could
    // fix that, but I haven't decided if that's desirable yet).
    Assert(Stack->Length != 0);

    return Stack->Data[--Stack->Length];
}

template <typename T>
static inline
T Peek(stack<T> *Stack)
{
    // NOTE[joe] We can't pop from an empty stack, so we'll assert here.
    // Unfortunately our assertions don't cause the program to crash (we could
    // fix that, but I haven't decided if that's desirable yet).
    Assert(Stack->Length != 0);

    return Stack->Data[Stack->Length - 1];
}

template <typename T>
static inline
void Empty(stack<T> *Stack)
{
    // NOTE[joe] Can't empty an already empty stack.
    // Unfortunately our assertions don't cause the program to crash (we could
    // fix that, but I haven't decided if that's desirable yet).
    Assert(Stack->Data != 0);

    free(Stack->Data);

    *Stack = { };
}
