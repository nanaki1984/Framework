#include "Render/Image/Bits.h"

namespace Framework {
    namespace Bits {

uint32_t ChannelMax(uint32_t offset, uint32_t size)
{
    return (1 << size) - 1;
}

uint32_t ChannelMask(uint32_t offset, uint32_t size)
{
    return ((1 << size) - 1) << offset;
}

uint32_t FloatToInt(float src,
                    uint32_t dstOffset, uint32_t dstSize)
{
    uint32_t dst = (uint32_t)(src * (float)ChannelMax(dstOffset, dstSize));
    dst &= ChannelMax(dstOffset, dstSize);
    dst <<= dstOffset;

    return dst;
}

float IntToFloat(uint32_t src,
                 uint32_t srcOffset, uint32_t srcSize)
{
    return (float)src / (float)ChannelMax(srcOffset, srcSize);
}

uint32_t IntToInt(uint32_t src,
                  uint32_t srcOffset, uint32_t srcSize,
                  uint32_t dstOffset, uint32_t dstSize)
{
    src &=  ChannelMask(srcOffset, srcSize);
    src >>= srcOffset;

    uint32_t dst = (src * ChannelMax(dstOffset, dstSize)) /
                          ChannelMax(srcOffset, srcSize);
    dst <<= dstOffset;

    return dst;
}

void CheckOffset(uint32_t offset,
                 uint32_t& new_offset,
                 uint32_t& dword_offset)
{
    dword_offset = offset / 32;
    new_offset   = offset % 32;
}

    } // namespace Bits
} // namespace Framework
