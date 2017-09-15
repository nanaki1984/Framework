#include "Core/String.h"
#include "Core/StringHash.h"
#include "Core/Collections/Array.h"
#include "Core/Memory/MallocAllocator.h"
#include <cstring>

namespace Framework {

const String String::Empty = String();

String::String()
: heapBuffer(Memory::GetAllocator<MallocAllocator>()),
  length(0)
{
    localBuffer[0] = '\0';
}

String::String(const char *string)
: heapBuffer(Memory::GetAllocator<MallocAllocator>()),
  length(0)
{
    this->Set(string);
}

String::String(Allocator &allocator)
: heapBuffer(allocator),
  length(0)
{
    localBuffer[0] = '\0';
}

String::String(Allocator &allocator, uint32_t initialSize)
: heapBuffer(allocator),
  length(0)
{
    localBuffer[0] = '\0';
    if (initialSize >= kLocalBufferSize)
        heapBuffer.Reserve(initialSize + 1);
}

String::String(Allocator &allocator, const char *string)
: heapBuffer(allocator),
  length(0)
{
    this->Set(string);
}

String::String(const String &other)
: heapBuffer(other.heapBuffer),
  length(other.length)
{
    if (heapBuffer.IsEmpty()) {
        if (length > 0)
            Memory::Copy(localBuffer, other.localBuffer, length + 1);
        else
            localBuffer[0] = '\0';
    }
}

String::String(String &&other)
: heapBuffer(std::forward<Array<char>>(other.heapBuffer)),
  length(other.length)
{
    if (heapBuffer.IsEmpty()) {
        if (length > 0)
            Memory::Move(localBuffer, other.localBuffer, length + 1);
        else
            localBuffer[0] = '\0';
    }

    other.length = 0;
    other.localBuffer[0] = '\0';
}

String::~String()
{ }

String&
String::operator =(const char *string)
{
    this->Set(string);
    return (*this);
}

String&
String::operator =(const String &other)
{
    this->Set(other.AsCString());
    return (*this);
}

String&
String::operator =(String &&other)
{
    heapBuffer = std::forward<Array<char>>(other.heapBuffer);
    length = other.length;
    if (length > 0 && heapBuffer.IsEmpty())
        Memory::Move(localBuffer, other.localBuffer, length + 1);

    other.length = 0;
    other.localBuffer[0] = '\0';

    return (*this);
}

bool
String::operator ==(const String &other) const
{
    return 0 == strcmp(this->AsCString(), other.AsCString());
}

bool
String::operator !=(const String &other) const
{
    return 0 != strcmp(this->AsCString(), other.AsCString());
}

bool
String::operator >=(const String &other) const
{
    return strcmp(this->AsCString(), other.AsCString()) >= 0;
}

bool
String::operator <=(const String &other) const
{
    return strcmp(this->AsCString(), other.AsCString()) <= 0;
}

bool
String::operator >(const String &other) const
{
    return strcmp(this->AsCString(), other.AsCString()) > 0;
}

bool
String::operator <(const String &other) const
{
    return strcmp(this->AsCString(), other.AsCString()) < 0;
}

bool
String::operator ==(const char *other) const
{
    return 0 == strcmp(this->AsCString(), other);
}

bool
String::operator !=(const char *other) const
{
    return 0 != strcmp(this->AsCString(), other);
}

bool
String::operator >=(const char *other) const
{
    return strcmp(this->AsCString(), other) >= 0;
}

bool
String::operator <=(const char *other) const
{
    return strcmp(this->AsCString(), other) <= 0;
}

bool
String::operator >(const char *other) const
{
    return strcmp(this->AsCString(), other) > 0;
}

bool
String::operator <(const char *other) const
{
    return strcmp(this->AsCString(), other) < 0;
}

String
String::operator +(const String &other) const
{
    String newString(this->GetAllocator(), length + other.length + 1);
    newString.Set(this->AsCString());
    newString.Append(other);
    return newString;
}

String&
String::operator +=(const String &other)
{
    this->Append(other);
    return (*this);
}

String
String::operator +(const char *other) const
{
    String newString(this->GetAllocator(), length + strlen(other) + 1);
    newString.Set(this->AsCString());
    newString.Append(other);
    return newString;
}

String&
String::operator +=(const char *other)
{
    this->Append(other);
    return (*this);
}

String
String::operator +(char other) const
{
    String newString(this->GetAllocator(), length + 2);
    newString.Set(this->AsCString());
    newString.Append(other);
    return newString;
}

String&
String::operator +=(char other)
{
    this->Append(other);
    return (*this);
}

const char&
String::operator [](uint32_t index) const
{
    assert(index <= length);
    return this->AsCString()[index];
}

char&
String::operator [](uint32_t index)
{
    assert(index <= length);
    return this->AsCString()[index];
}

String::operator StringHash() const
{
    return StringHash::FromCString(this->AsCString());
}

const char*
String::AsCString() const
{
    return heapBuffer.IsEmpty() ? localBuffer : heapBuffer.Begin();
}

char*
String::AsCString()
{
    return heapBuffer.IsEmpty() ? localBuffer : heapBuffer.Begin();
}

Allocator&
String::GetAllocator() const
{
    return heapBuffer.GetAllocator();
}

uint32_t
String::Length() const
{
    return length;
}

bool
String::IsEmpty() const
{
    return 0 == length;
}

void
String::Clear()
{
    heapBuffer.Clear();
    localBuffer[0] = '\0';
    length = 0;
}

void
String::Reserve(uint32_t newSize)
{
    if (newSize < kLocalBufferSize)
        return;
    heapBuffer.Reserve(newSize);
}

void
String::Set(const char* string)
{
    uint32_t len = strlen(string);
    if (len < kLocalBufferSize) {
        if (!heapBuffer.IsEmpty())
            heapBuffer = Array<char>(this->GetAllocator());
        Memory::Copy(localBuffer, string, len + 1);
    } else {
        heapBuffer.Clear();
        heapBuffer.InsertRange(0, string, len + 1);
    }
    length = len;
}

void
String::Append(const String &string)
{
    uint32_t extraLen = string.length;
    if ((length + extraLen + 1) < kLocalBufferSize) {
        if (!heapBuffer.IsEmpty())
            heapBuffer = Array<char>(this->GetAllocator());
        Memory::Copy(localBuffer + length, string.AsCString(), extraLen + 1);
    } else {
        if (heapBuffer.IsEmpty())
            heapBuffer.InsertRange(0, localBuffer, length + 1);
        heapBuffer.InsertRange(length, string.AsCString(), extraLen);
    }
    length += extraLen;
}

void
String::Append(const char* string)
{
    uint32_t extraLen = strlen(string);
    if ((length + extraLen + 1) < kLocalBufferSize) {
        if (!heapBuffer.IsEmpty())
            heapBuffer = Array<char>(this->GetAllocator());
        Memory::Copy(localBuffer + length, string, extraLen + 1);
    } else {
        if (heapBuffer.IsEmpty())
            heapBuffer.InsertRange(0, localBuffer, length + 1);
        heapBuffer.InsertRange(length, string, extraLen);
    }
    length += extraLen;
}

void
String::Append(const char* string, uint32_t count)
{
    if (0 == count)
        return;
    if ((length + count + 1) < kLocalBufferSize) {
        if (!heapBuffer.IsEmpty())
            heapBuffer = Array<char>(this->GetAllocator());
        Memory::Copy(localBuffer + length, string, count);
        localBuffer[length + count] = '\0';
    } else {
        if (heapBuffer.IsEmpty())
            heapBuffer.InsertRange(0, localBuffer, length + 1);
        heapBuffer.InsertRange(length, string, count);
    }
    length += count;
}

void
String::Append(char c)
{
    if ((length + 2) < kLocalBufferSize) {
        if (!heapBuffer.IsEmpty())
            heapBuffer = Array<char>(this->GetAllocator());
        localBuffer[length] = c;
        localBuffer[length + 1] = '\0';
    } else {
        if (heapBuffer.IsEmpty())
            heapBuffer.InsertRange(0, localBuffer, length + 1);
        heapBuffer.Insert(length, c);
    }
    ++length;
}

int32_t
String::IndexOf(char c) const
{
    return this->IndexOf(c, 0, length);
}

int32_t
String::IndexOf(char c, uint32_t index) const
{
    return this->IndexOf(c, index, length - index);
}

int32_t
String::IndexOf(char c, uint32_t index, uint32_t count) const
{
    if (0 == length)
        return -1;
    assert(index < length);
    uint32_t lastIndex = index + count;
    assert(lastIndex <= length);
    for (uint32_t i = index; i < lastIndex; ++i) {
        if ((*this)[i] == c)
            return i;
    }
    return -1;
}

int32_t
String::LastIndexOf(char c) const
{
    return this->LastIndexOf(c, length - 1, length);
}

int32_t
String::LastIndexOf(char c, uint32_t index) const
{
    return this->LastIndexOf(c, index, index);
}

int32_t
String::LastIndexOf(char c, uint32_t index, uint32_t count) const
{
    assert(index < length);
    uint32_t firstIndex = index - count + 1;
    assert(firstIndex < length);
    for (uint32_t i = index; i >= firstIndex; --i) {
        if ((*this)[i] == c)
            return i;
    }
    return -1;
}

String
String::Substring(uint32_t index) const
{
    return this->Substring(index, length - index);
}

String
String::Substring(uint32_t index, uint32_t count) const
{
    assert(index < length);
    assert((index + count) <= length);

    String s(this->GetAllocator(), count);
    s.Append(this->AsCString() + index, count);

    return s;
}

void
String::Replace(char oldChar, char newChar)
{
    int32_t index = this->IndexOf(oldChar);
    while (index != -1) {
        (*this)[index] = newChar;
        index = this->IndexOf(oldChar, index + 1);
    }
}

Array<String>
String::Split(const char *chars) const
{
    Array<String> strings(this->GetAllocator());

    uint32_t strSz = length + 1;
    char *str = static_cast<char*>(this->GetAllocator().Allocate(strSz, __alignof(char)));
    Memory::Copy(str, this->AsCString(), strSz);

    char *ptr = strtok(str, chars);
    while (ptr != nullptr) {
        strings.PushBack(ptr);
        strtok(nullptr, chars);
    }

    return strings;
}

} // namespace Framework
