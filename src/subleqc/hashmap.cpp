// @file hashmap.cpp
// @author Joseph Miles <josephmiles2015@gmail.com>
// @date 2019-10-03
//
// This file contains our definitions for our hashmap, used by the SUBLEQ
// assembler to store address values for labels.

#pragma once

// C Standard Libraries
#include <cstring>
#include <algorithm>

// Own Libraries
#include "util.cpp"

#define MAX_PAGE_SIZE 64

struct hashnode
{
    const char *Key;
    unsigned int Value;
};

struct hashmap
{
    hashnode Nodes[MAX_PAGE_SIZE];
    hashmap *NextPage;
    unsigned int ActiveNodeCount;
};


// Hash() uses the djib2 string hashing function to create a hash value for
// the input string. It then divides the computed hash by the maximum page size
// to compute the table index the hashed value belongs at.
unsigned int Hash(const char *String)
{
    unsigned int Hash = 5381;
    char c;

    while ((c = *String++))
        Hash = ((Hash << 5) + Hash) + (unsigned int)c;

    return Hash % MAX_PAGE_SIZE;
}

// Put() computes the hash of the [Key] and tries to insert the value into the
// first page of the given [Map]. If the value cannot be inserted, it repeats
// for the next page, or creates a new page if there is no next page.
void Put(hashmap *Map, const char *Key, const unsigned int Value)
{
    if (Map->ActiveNodeCount < (MAX_PAGE_SIZE - 1))
    {
        int KeyLength = std::strlen(Key);

        char* KeyCopy = new char[KeyLength + 1];
        std::fill(KeyCopy, KeyCopy + KeyLength + 1, '\0');
        std::copy(Key, Key + KeyLength, KeyCopy);

        unsigned int Index = Hash(Key);

        if (Map->Nodes[Index].Key == NULL)
        {
            Map->Nodes[Index] = (hashnode) {
                .Key = KeyCopy,
                .Value = Value
            };
        }

        else
        {
            unsigned int i = Index + 1;
            while (i != Index)
            {
                if (Map->Nodes[i].Key == NULL)
                {
                    Map->Nodes[i] = (hashnode) {
                        .Key = KeyCopy,
                        .Value = Value
                    };

                    break;
                }

                if (++i == MAX_PAGE_SIZE)
                    i = 0;
            }
        }

        Map->ActiveNodeCount++;
    }
    else if (Map->NextPage == NULL)
    {
        Map->NextPage = new hashmap();
        *(Map->NextPage) = { };

        Put(Map->NextPage, Key, Value);
    }
    else
    {
        Put(Map->NextPage, Key, Value);
    }
}

// Get() takes a hashmap pointer [Map] and a string [Key] and returns the value
// stored in [Map] that is associated with that [Key].
status<unsigned int> Get(hashmap *Map, const char *Key)
{
    // todo(jrm): Replace with a hash-and-grab-or-search method instead of just
    // linearly searching the hash table like a hooligan.
    for (unsigned int i = 0; i < MAX_PAGE_SIZE; i++)
    {
        const char *StoredKey = Map->Nodes[i].Key;

        if (StoredKey == NULL)
        {
            return (status<unsigned int>) {
                .Status = ERROR,
                .Value  = 0
            };
        }
        else if (std::strcmp(StoredKey, Key) == 0)
        {
            return (status<unsigned int>) {
                .Status = OK,
                .Value  = Map->Nodes[i].Value
            };
        }
    }

    // NOTE[joe] This is bad form, as 0 is a valid value to be stored in the
    // hash map.
    if (Map->NextPage != NULL)
        return Get(Map, Key);

    else {
        return (status<unsigned int>) {
            .Status = ERROR,
            .Value  = 0
        };
    }
}

#undef MAX_PAGE_SIZE
