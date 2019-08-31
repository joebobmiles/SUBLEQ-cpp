/**
 * @file buffer.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-08-11
 *
 * This file is contains all the declarations and functionality for our dynamic
 * buffer struct.
 */

#pragma once

template <typename T>
struct buffer {
    T *Data;
    unsigned int Length;
    unsigned int _Size;

    T& operator[](unsigned int Index)
    {
        return Data[Index];
    }
};


template <typename T>
static
void Append(buffer<T> *Buffer, const T Value)
{
    if (Buffer->_Size == 0)
    {
        Buffer->_Size  = 1;
        Buffer->Length = 0;
        Buffer->Data = (T *)malloc(sizeof(T) * Buffer->_Size);
    }
    else if (Buffer->Length == Buffer->_Size)
    {
        Buffer->_Size *= 2;
        Buffer->Data = (T *)realloc(Buffer->Data,
                                    sizeof(T) * Buffer->_Size);
    }

    Buffer->Data[Buffer->Length++] = Value;
}

template <typename T>
static inline
void Empty(buffer<T> *Buffer)
{
    Buffer->_Size = 0;
    Buffer->Length = 0;

    free(Buffer->Data);
    Buffer->Data = 0;
}
