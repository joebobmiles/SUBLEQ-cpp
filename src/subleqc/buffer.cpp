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
        Buffer->Data = new T[Buffer->_Size];
    }
    else if (Buffer->Length == Buffer->_Size)
    {
        Buffer->_Size *= 2;

        T *Temp = new T[Buffer->_Size];

        for (unsigned int i = 0; i < Buffer->Length; i++)
        {
            Temp[i] = Buffer->Data[i];
        }

        delete[] Buffer->Data;
        Buffer->Data = Temp;
    }

    Buffer->Data[Buffer->Length++] = Value;
}

template <typename T>
static inline
void Empty(buffer<T> *Buffer)
{
    Buffer->_Size = 0;
    Buffer->Length = 0;

    delete[] Buffer->Data;
}
