#pragma once

#include <cstdint>
#include <cstring>

namespace Framework {

template <unsigned int N, unsigned int I>
struct FnvHash
{
    inline static unsigned int Hash(const char (&str)[N])
    {
        return (FnvHash<N, I-1>::Hash(str) ^ str[I-1])*16777619u;
    }
};

template <unsigned int N>
struct FnvHash<N, 1>
{
    inline static unsigned int Hash(const char (&str)[N])
    {
        return (2166136261u ^ str[0])*16777619u;
    }
};

struct StringHash
{
    uint32_t hash;
    uint32_t length;
    const char *string;

    template <unsigned int N>
    inline StringHash(const char (&str)[N])
    : hash(FnvHash<N, N-1>::Hash(str)),
      length(N - 1),
      string(str)
    { }

    StringHash();
    StringHash(uint32_t _hash, uint32_t _length, const char *_string);

    bool operator ==(const StringHash &other) const;
    bool operator !=(const StringHash &other) const;

    static StringHash FromCString(const char *str);
};

} // namespace Framework
