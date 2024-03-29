/**
 * @file stack.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-30
 *
 * This file contains our implementation of a generic stack.
 */

#pragma once


// C standard libraries

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
        Stack->_Size = 1;
        Stack->Data = new T[Stack->_Size];
    }
    else if (Stack->Length == Stack->_Size)
    {
        Stack->_Size *= 2;
        Stack->Data = (T *)realloc(Stack->Data, sizeof(T) * Stack->_Size);

        T *Temp = new T[Stack->_Size];

        for (unsigned int i = 0; i < Stack->Length; i++)
        {
            Temp[i] = Stack->Data[i];
        }

        delete[] Stack->Data;
        Stack->Data = Temp;
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
static
void Reverse(stack<T> *Stack)
{
    stack<T> Temp = { };

    while (Stack->Length > 0)
        Push<T>(&Temp, Pop(Stack));

    Empty<T>(Stack);
    *Stack = Temp;
}

template <typename T>
static inline
void Empty(stack<T> *Stack)
{
    // NOTE[joe] Can't empty an already empty stack.
    // Unfortunately our assertions don't cause the program to crash (we could
    // fix that, but I haven't decided if that's desirable yet).
    Assert(Stack->Data != 0);

    delete[] Stack->Data;

    *Stack = { };
}
