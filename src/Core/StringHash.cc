#include <cstring>
#include "Core/StringHash.h"
#include "Core/StringsTable.h"

namespace Framework {

StringHash::StringHash()
{ }

StringHash::StringHash(uint32_t _hash, uint32_t _length, const char *_string)
: hash(_hash),
  length(_length),
  string(_string)
{ }

bool
StringHash::operator ==(const StringHash &other) const
{
    return hash == other.hash &&
           (string == other.string || 0 == strcmp(string, other.string));
}

bool
StringHash::operator !=(const StringHash &other) const
{
    return !this->operator ==(other);
}

StringHash
StringHash::FromCString(const char *str)
{
    uint32_t hash = 2166136261u,
             length = strlen(str);
    for (uint32_t i = 0; i < length; ++i) {
        hash ^= str[i];
        hash *= 16777619u;
    }
    return StringHash(hash, length, StringsTable::Instance()->GetString(hash, str, length));
}

} // namespace Framework
