#pragma once

#include <string>
#include <algorithm>
#include "Core/String.h"
#include "Core/Memory/ScratchAllocator.h"

namespace Framework {

class BitStream {
public:
    typedef size_t BitSize;
private:
    static const unsigned int BufferSize = 16;

    static size_t BitsToBytes(BitSize bits);
    static BitSize BytesToBits(size_t bytes);
    static unsigned char BitMask(BitSize bitPos);

    Allocator *allocator;

    BitSize bitsUsed;
    BitSize bitsAllocated;
    bool    copyData;

    mutable BitSize bitsReadPos;

    unsigned char *data;
    unsigned char buffer[BufferSize];

    void Realloc(size_t newSize);
    void ReserveBits(BitSize numBits);
    void ReserveBytes(size_t numBytes);

    void WriteBits(const void *bits, BitSize numBits, bool swapNotAlignedBits = true);
    void ReadBits(void *bits, BitSize numBits, bool swapNotAlignedBits = true) const;

    void WriteBit(bool bit);
    bool ReadBit() const;
public:
    BitStream(Allocator &_allocator);
    BitStream(const BitStream &stream);
    BitStream(BitStream &&stream);
    BitStream(Allocator &_allocator, size_t initialSize);
    BitStream(Allocator &_allocator, const void *_data, size_t dataLength, bool _copyData = true);
    ~BitStream();

    BitStream& operator =(const BitStream &stream);
    BitStream& operator =(BitStream &&stream);

    const void* GetData() const;
    const void* GetReadPos() const;
    size_t GetSize() const;
    size_t GetCapacity() const;

    void Fill(const void *_data, size_t dataLength, bool _copyData = true);

    void Reset();
    void SeekBits(BitSize bits);
    void SeekBytes(size_t bytes);
    void Rewind() const;
    void SkipBits(BitSize bits) const;
    void SkipBytes(size_t bytes) const;
    bool EndOfStream() const;
    BitSize RemainingBits() const;
    size_t RemainingBytes() const;

    void Reserve(size_t size);
    void WriteBytes(const void *bytes, size_t numBytes);
    void ReadBytes(void *bytes, size_t numBytes);

    template <typename T>
    BitStream& operator <<(const T &value);

    template <typename T>
    const BitStream& operator >>(T &value) const;

    template <typename T>
    void Write(const T &value);

    template <typename T>
    void Read(T &value) const;
};

inline size_t
BitStream::BitsToBytes(BitSize bits)
{
    return (bits >> 3) + ((bits & 0x7) ? 1 : 0);
}

inline BitStream::BitSize
BitStream::BytesToBits(size_t bytes)
{
    return bytes << 3;
}

inline unsigned char
BitStream::BitMask(BitSize bitPos)
{
    return (0x80 >> (bitPos & 0x7));
}

inline const void*
BitStream::GetData() const
{
    return static_cast<const void*>(data);
}

inline const void*
BitStream::GetReadPos() const
{
    return static_cast<const void*>(data + (bitsReadPos >> 3));
}

inline size_t
BitStream::GetSize() const
{
    return BitsToBytes(bitsUsed);
}

inline size_t
BitStream::GetCapacity() const
{
    return BitsToBytes(bitsAllocated);
}

inline void
BitStream::Reset()
{
    bitsUsed    = 0;
    bitsReadPos = 0;
}

inline void
BitStream::SeekBits(BitSize bits)
{
    bitsUsed = 0;
    this->Reserve(BitsToBytes(bits));
    bitsUsed = bits;
}

inline void
BitStream::SeekBytes(size_t bytes)
{
    bitsUsed = 0;
    this->Reserve(bytes);
    bitsUsed = BytesToBits(bytes);
}

inline void
BitStream::Rewind() const
{
    bitsReadPos = 0;
}

inline void
BitStream::SkipBits(BitSize bits) const
{
    bitsReadPos = std::min(bitsReadPos + bits, bitsUsed);
}

inline void
BitStream::SkipBytes(size_t bytes) const
{
    bitsReadPos = std::min(bitsReadPos + BytesToBits(bytes), bitsUsed);
}

inline bool
BitStream::EndOfStream() const
{
    return (bitsReadPos == bitsUsed);
}

inline BitStream::BitSize
BitStream::RemainingBits() const
{
    return bitsUsed - bitsReadPos;
}

inline size_t
BitStream::RemainingBytes() const
{
    return BitsToBytes(bitsUsed - bitsReadPos);
}

inline void
BitStream::Reserve(size_t size)
{
    if (0 == size)
        return;

    BitSize newBitsAllocated = bitsUsed + BytesToBits(size);

    if (newBitsAllocated > bitsAllocated) {
        size_t newBytesAllocated = BitsToBytes(newBitsAllocated);
        if (newBytesAllocated > BufferSize)
            this->Realloc(newBytesAllocated);
    }
}

inline void
BitStream::WriteBytes(const void *bytes, size_t numBytes)
{
    this->WriteBits(bytes, BytesToBits(numBytes));
}

inline void
BitStream::ReadBytes(void *bytes, size_t numBytes)
{
    this->ReadBits(bytes, BytesToBits(numBytes));
}

template <typename T>
BitStream&
BitStream::operator <<(const T &value)
{
    this->WriteBits(static_cast<const void*>(&value), BytesToBits(sizeof(T)));
    return (*this);
}

template <>
inline BitStream&
BitStream::operator <<(const bool &value)
{
    this->WriteBit(value);
    return (*this);
}

template <>
inline BitStream&
BitStream::operator <<(const BitStream &value)
{
    this->WriteBits(static_cast<const void*>(&value.bitsUsed), sizeof(BitSize));
    this->WriteBits(static_cast<const void*>(value.data), value.bitsUsed, false);
    return (*this);
}

template <>
inline BitStream&
BitStream::operator <<(const String &value)
{
    uint32_t sz = value.Length();
    (*this) << sz;
    for (uint32_t i = 0; i < sz; ++i)
        (*this) << value[i];
    return (*this);
}

template <typename T>
const BitStream&
BitStream::operator >>(T &value) const
{
    this->ReadBits((void*)&value, BytesToBits(sizeof(T)));
    return (*this);
}

template <>
inline const BitStream&
BitStream::operator >>(bool &value) const
{
    value = this->ReadBit();
    return (*this);
}

template <>
inline const BitStream&
BitStream::operator >>(BitStream &value) const
{
    BitSize bitsToRead;
    (*this) >> bitsToRead;

    auto &scratchAlloc = Memory::GetAllocator<ScratchAllocator>();

    void *buffer = scratchAlloc.Allocate(BitsToBytes(bitsToRead), 1);
    this->ReadBits(buffer, bitsToRead, false);
    value.WriteBits(buffer, bitsToRead, false);

    scratchAlloc.Free(buffer);

    return (*this);
}

template <>
inline const BitStream&
BitStream::operator >>(String &value) const
{
    uint32_t sz;
    (*this) >> sz;
    value.Clear();
    value.Reserve(sz);
    for (uint32_t i = 0; i < sz; ++i)
    {
        char c;
        (*this) >> c;
        value.Append(c);
    }
    return (*this);
}

template <typename T>
void
BitStream::Write(const T &value)
{
    (*this) << value;
}

template <typename T>
void
BitStream::Read(T &value) const
{
    (*this) >> value;
}

} // namespace Framework
