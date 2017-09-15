#pragma once

#include <cstring>
#include <iterator>
#include "Core/Debug.h"

template <typename T>
struct EnumStrings { };

template <typename T>
const char* EnumToString(T value)
{
    return EnumStrings<T>::strings[value];
}

template <typename T>
T StringToEnum(const char *string)
{
    auto begin = std::begin(EnumStrings<T>::strings),
         end   = std::end(EnumStrings<T>::strings);
    for (auto it = begin; it != end; ++it) {
        if (0 == strcmp(*it, string))
            return static_cast<T>(std::distance(begin, it));
    }
    assert(false);
    return T();
}

#define DeclareEnumStrings(type,count) \
template <> \
struct EnumStrings<type> \
{ \
    static const char* strings[count]; \
}

#define DefineEnumStrings(type) \
const char* EnumStrings<type>::strings[]
