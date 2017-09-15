#include "Core/StringsTable.h"
#include "Core/Collections/Hash.h"

namespace Framework {

DefineClassInfo(Framework::StringsTable, Framework::RefCounted);

StringsTable::StringsTable(Allocator *allocator)
: hashTable(*allocator)
{ }

StringsTable::~StringsTable()
{
    for (auto it = hashTable.Begin(), end = hashTable.End(); it != end; ++it)
        hashTable.GetAllocator().Free(const_cast<char *>(it->string));
}

const char*
StringsTable::GetString(uint32_t hash, const char *string, uint32_t length)
{
    StringHash *item = hashTable.Get(hash);
    while (item != nullptr) {
        if (item->length == length && 0 == strcmp(item->string, string))
            break;
        item = hashTable.Next(item);
    }

    if (nullptr == item) {
        char *copyStr = static_cast<char *>(hashTable.GetAllocator().Allocate((length + 1) * sizeof(char), __alignof(char)));
        memcpy(copyStr, string, length);
        copyStr[length] = '\0';
        hashTable.Add(hash, StringHash(hash, length, copyStr));
        return copyStr;
    } else {
        return item->string;
    }
}

} // namespace Framework
