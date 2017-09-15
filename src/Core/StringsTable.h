#pragma once

#include "Core/Singleton.h"
#include "Core/Collections/Hash_type.h"
#include "Core/StringHash.h"

namespace Framework {

class StringsTable : public Singleton<StringsTable> {
    DeclareClassInfo;
protected:
    Hash<StringHash> hashTable;
public:
    StringsTable(Allocator *allocator);
    StringsTable(const StringsTable &other) = delete;
    virtual ~StringsTable();

    const char* GetString(uint32_t hash, const char *string, uint32_t length);

    StringsTable& operator =(const StringsTable &other) = delete;
};

} // namespace Framework
