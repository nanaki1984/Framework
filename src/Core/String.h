#pragma once

#include "Core/Collections/Array_type.h"

namespace Framework {

struct StringHash;

class String {
protected:
    static const int kLocalBufferSize = 16;

    char localBuffer[kLocalBufferSize];
    Array<char> heapBuffer;
    uint32_t length;
public:
    static const String Empty;

    String();
    String(const char *string);
    String(Allocator &allocator);
    String(Allocator &allocator, uint32_t initialSize);
    String(Allocator &allocator, const char *string);
    String(const String &other);
    String(String &&other);
    ~String();

    String& operator =(const char *string);
    String& operator =(const String &other);
    String& operator =(String &&other);

    bool operator ==(const String &other) const;
    bool operator !=(const String &other) const;
    bool operator >=(const String &other) const;
    bool operator <=(const String &other) const;
    bool operator >(const String &other) const;
    bool operator <(const String &other) const;

    bool operator ==(const char *other) const;
    bool operator !=(const char *other) const;
    bool operator >=(const char *other) const;
    bool operator <=(const char *other) const;
    bool operator >(const char *other) const;
    bool operator <(const char *other) const;

    String operator +(const String &other) const;
    String& operator +=(const String &other);

    String operator +(const char *other) const;
    String& operator +=(const char *other);

    String operator +(char other) const;
    String& operator +=(char other);

    const char& operator [](uint32_t index) const;
    char& operator [](uint32_t index);

    operator StringHash() const;

    const char* AsCString() const;
    char* AsCString();

    Allocator& GetAllocator() const;
    uint32_t Length() const;
    bool IsEmpty() const;
    void Clear();
    void Reserve(uint32_t newSize);
    void Set(const char* string);
    void Append(const String &string);
    void Append(const char* string);
    void Append(const char* string, uint32_t count);
    void Append(char c);

    int32_t IndexOf(char c) const;
    int32_t IndexOf(char c, uint32_t index) const;
    int32_t IndexOf(char c, uint32_t index, uint32_t count) const;

    int32_t LastIndexOf(char c) const;
    int32_t LastIndexOf(char c, uint32_t index) const;
    int32_t LastIndexOf(char c, uint32_t index, uint32_t count) const;

    String Substring(uint32_t index) const;
    String Substring(uint32_t index, uint32_t count) const;

    void Replace(char oldChar, char newChar);

    Array<String> Split(const char *chars) const;
};

} // namespace Framework
