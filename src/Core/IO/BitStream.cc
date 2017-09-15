#include "Core/IO/BitStream.h"
#include "Core/Debug.h"
#include "Core/Memory/Memory.h"

namespace Framework {

BitStream::BitStream(Allocator &_allocator)
: allocator    (&_allocator),
  bitsUsed     (0),
  bitsAllocated(BytesToBits(BufferSize)),
  copyData     (false),
  bitsReadPos  (0),
  data         (buffer)
{ }

BitStream::BitStream(const BitStream &stream)
: allocator    (stream.allocator),
  bitsUsed     (0),
  bitsAllocated(BytesToBits(BufferSize)),
  copyData     (false),
  bitsReadPos  (0),
  data         (buffer)
{
    this->Fill(stream.GetData(), stream.GetSize());
}

BitStream::BitStream(BitStream &&stream)
: allocator    (stream.allocator),
  bitsUsed     (stream.bitsUsed),
  bitsAllocated(stream.bitsAllocated),
  copyData     (stream.copyData),
  bitsReadPos  (0),
  data         (buffer)
{
    if (stream.data == stream.buffer)
        Memory::Copy(buffer, stream.buffer, BufferSize);
    else
        data = stream.data;

    stream.bitsUsed      = 0;
    stream.bitsAllocated = BytesToBits(BufferSize);
    stream.copyData       = false;
    stream.bitsReadPos    = 0;
    stream.data           = stream.buffer;
}

BitStream::BitStream(Allocator &_allocator, size_t initialSize)
: allocator(&_allocator)
{
    if (initialSize <= BufferSize) {
        bitsUsed      = 0;
        bitsAllocated = BytesToBits(BufferSize);
        bitsReadPos   = 0;
        copyData      = false;
        data          = buffer;
    } else {
        bitsUsed      = 0;
        bitsAllocated = BytesToBits(initialSize);
        bitsReadPos   = 0;
        copyData      = false;
        data          = static_cast<unsigned char*>(allocator->Allocate(initialSize, 1));
    }
}

BitStream::BitStream(Allocator &_allocator, const void *_data, size_t dataLength, bool _copyData)
: allocator(&_allocator),
  copyData (_copyData)
{
    if (dataLength <= BufferSize && copyData) {
        bitsUsed      = BytesToBits(dataLength);
        bitsAllocated = BytesToBits(BufferSize);
        bitsReadPos   = 0;
        data          = buffer;
    } else {
        bitsUsed      = BytesToBits(dataLength);
        bitsAllocated = BytesToBits(dataLength);
        bitsReadPos   = 0;

        if (copyData)
            data = static_cast<unsigned char*>(allocator->Allocate(dataLength, 1));
        else
            data = static_cast<unsigned char*>(const_cast<void*>(_data));
    }

    if (copyData)
        Memory::Copy(static_cast<void*>(data), _data, dataLength);
}

BitStream::~BitStream()
{
    if (copyData && data != buffer)
        allocator->Free(data);
}

BitStream&
BitStream::operator =(const BitStream &stream)
{
    this->Fill(stream.GetData(), stream.GetSize());
    return (*this);
}

BitStream&
BitStream::operator =(BitStream &&stream)
{
    if (copyData && data != buffer)
        allocator->Free(data);

    allocator     = stream.allocator;
    bitsUsed      = stream.bitsUsed;
    bitsAllocated = stream.bitsAllocated;
    copyData      = stream.copyData;
    bitsReadPos   = 0;
    data          = buffer;

    if (stream.data == stream.buffer)
        Memory::Copy(buffer, stream.buffer, BufferSize);
    else
        data = stream.data;

    stream.bitsUsed      = 0;
    stream.bitsAllocated = BytesToBits(BufferSize);
    stream.copyData      = false;
    stream.bitsReadPos   = 0;
    stream.data          = stream.buffer;

    return (*this);
}

void
BitStream::Realloc(size_t newSize)
{
    assert(newSize > BufferSize);

    unsigned char *oldData = data;
    bool oldCopyData = copyData;
    size_t oldSize = BitsToBytes(bitsAllocated);

    copyData = true;

    if (newSize <= BufferSize) {
        bitsAllocated = BytesToBits(BufferSize);
        data          = buffer;
    } else {
        bitsAllocated = BytesToBits(newSize);
        data          = static_cast<unsigned char*>(allocator->Allocate(newSize, 1));
    }
    
    if (data != oldData)
        Memory::Copy(data, oldData, newSize < oldSize ? newSize : oldSize);

    if (oldCopyData && oldData != buffer)
        allocator->Free(oldData);
}

void
BitStream::ReserveBits(BitSize numBits)
{
    if (0 == numBits)
        return;

    BitSize newBitsAllocated = bitsUsed + numBits;

    if (newBitsAllocated > bitsAllocated)
        this->Realloc(std::max(BitsToBytes(newBitsAllocated), BitsToBytes(bitsAllocated) * 2));
}

void
BitStream::ReserveBytes(size_t numBytes)
{
    if (0 == numBytes)
        return;
    
    BitSize newBitsAllocated = bitsUsed + BytesToBits(numBytes);
    
    if (newBitsAllocated > bitsAllocated)
        this->Realloc(std::max(BitsToBytes(newBitsAllocated), BitsToBytes(bitsAllocated) * 2));
}

void
BitStream::WriteBits(const void *bits, BitSize numBits, bool swapNotAlignedBits)
{
    this->ReserveBits(numBits);

    BitSize        dstBitsOffset = bitsUsed & 0x7;
    unsigned char *dstByte       = data + (bitsUsed >> 3);

    BitSize              bitsToWrite = numBits;
    const unsigned char *srcByte     = static_cast<const unsigned char*>(bits);

    while (bitsToWrite > 0) {
        if (bitsToWrite >= 8) {
            if (0 == dstBitsOffset) {
                *dstByte = *srcByte;
                ++dstByte;
            } else {
                *dstByte |= (*srcByte >> dstBitsOffset);
                ++dstByte;
                *dstByte = (*srcByte << (8 - dstBitsOffset));
            }

            bitsToWrite -= 8;
            bitsUsed    += 8;
        } else {
            unsigned char srcByteCopy = *srcByte;

            if (swapNotAlignedBits)
                srcByteCopy <<= (8 - bitsToWrite);

            if (0 == dstBitsOffset) {
                *dstByte = srcByteCopy;
            } else {
                *dstByte |= (srcByteCopy >> dstBitsOffset);
                ++dstByte;
                *dstByte = (srcByteCopy << (8 - dstBitsOffset));
            }

            bitsUsed    += bitsToWrite;
            bitsToWrite  = 0;
        }

        ++srcByte;
    }
}

void
BitStream::ReadBits(void *bits, BitSize numBits, bool swapNotAlignedBits) const
{
    BitSize        srcBitsOffset = bitsReadPos & 0x7;
    unsigned char *srcByte       = data + (bitsReadPos >> 3);
    
    BitSize        bitsToRead = numBits;
    unsigned char *dstByte    = static_cast<unsigned char*>(bits);

    while (bitsToRead > 0 && bitsReadPos < bitsUsed) {
        unsigned char byteRead;

        if (bitsToRead >= 8) {
            if (0 == srcBitsOffset) {
                byteRead = *srcByte;
                ++srcByte;
            } else {
                byteRead = (*srcByte << srcBitsOffset);
                ++srcByte;
                byteRead |= (*srcByte >> (8 - srcBitsOffset));
            }

            bitsToRead -= 8;
            bitsReadPos = std::min(bitsReadPos + 8, bitsUsed);
        } else {
            if (0 == srcBitsOffset) {
                byteRead = *srcByte;
                ++srcByte;
            } else {
                byteRead = (*srcByte << srcBitsOffset);
                ++srcByte;
                byteRead |= (*srcByte >> (8 - srcBitsOffset));
            }

            if (swapNotAlignedBits)
                byteRead >>= (8 - bitsToRead);

            bitsReadPos = std::min(bitsReadPos + bitsToRead, bitsUsed);
            bitsToRead  = 0;
        }

        *dstByte = byteRead;
        ++dstByte;
    }

    assert(0 == bitsToRead);
}

void
BitStream::WriteBit(bool bit)
{
    this->ReserveBits(1);
    if (0 == (bitsUsed & 0x7))
        *(data + (bitsUsed >> 3)) = 0;
    if (bit)
        *(data + (bitsUsed >> 3)) |= BitMask(bitsUsed);
    ++bitsUsed;
}

bool
BitStream::ReadBit() const
{
    assert(!this->EndOfStream());
    bool bit = (*(data + (bitsReadPos >> 3)) & BitMask(bitsReadPos) ? true : false);
    ++bitsReadPos;
    return bit;
}

void
BitStream::Fill(const void *_data, size_t dataLength, bool _copyData)
{
    if (copyData && data != buffer)
        allocator->Free(data);

    copyData = _copyData;

    if (dataLength <= BufferSize && copyData) {
        bitsUsed      = BytesToBits(dataLength);
        bitsAllocated = BytesToBits(BufferSize);
        bitsReadPos   = 0;
        data          = buffer;
    } else {
        bitsUsed      = BytesToBits (dataLength);
        bitsAllocated = BytesToBits (dataLength);
        bitsReadPos   = 0;

        if (copyData)
            data = static_cast<unsigned char*>(allocator->Allocate(dataLength, 1));
        else
            data = static_cast<unsigned char*>(const_cast<void*>(_data));
    }

    if (copyData)
        Memory::Copy(static_cast<void*>(data), _data, dataLength);
}

} // namespace Framework
