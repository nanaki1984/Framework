#pragma once

#include <cstdint>

namespace Framework {
    namespace Bits {

uint32_t ChannelMax(uint32_t offset, uint32_t size);
uint32_t ChannelMask(uint32_t offset, uint32_t size);

uint32_t FloatToInt(float src,
                    uint32_t dstOffset, uint32_t dstSize);
float IntToFloat(uint32_t src,
                 uint32_t srcOffset, uint32_t srcSize);
uint32_t IntToInt(uint32_t src,
                  uint32_t srcOffset, uint32_t srcSize,
                  uint32_t dstOffset, uint32_t dstSize);

void CheckOffset(uint32_t offset,
                 uint32_t& new_offset,
                 uint32_t& dword_offset);

    } // namespace Bits
} // namespace Framework
