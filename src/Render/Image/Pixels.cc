#include "Render/Image/Pixels.h"
#include "Render/Image/Bits.h"
#include "Math/Math.h"
#include <algorithm>

namespace Framework {
    namespace Pixels {

void PackFloats(void *dstPixel,
                const float *srcPixel,
                const ImageFormat &dstFrmt)
{
    std::fill((uint8_t*)dstPixel, (uint8_t*)dstPixel + dstFrmt.GetPixelSize(), 0);

    uint32_t new_offset;
    uint32_t dword_offset;

    int i;
    if (dstFrmt.GetFlags() & ImageFormat::IntegerType) { // float -> int
        uint32_t tmp;
        for (i = 0; i < 4; ++i) {
            if (0 == dstFrmt.GetChannelSize(i)) continue;

            Bits::CheckOffset(dstFrmt.GetChannelOffset(i), new_offset, dword_offset);

            tmp = Bits::FloatToInt(srcPixel[i], new_offset, dstFrmt.GetChannelSize(i));

            *((uint32_t*)dstPixel + dword_offset) |= tmp;
        }
    } else if ( dstFrmt.GetFlags() & ImageFormat::HalfFloatType ) { // float -> half float
        uint32_t tmp;
        for (i = 0; i < 4; ++i) {
            if (0 == dstFrmt.GetChannelSize(i)) continue;

            Bits::CheckOffset(dstFrmt.GetChannelOffset(i), new_offset, dword_offset);

            tmp = Math::FloatToHalf(*(uint32_t*)&srcPixel[i]);
            tmp <<= new_offset;

            *((uint32_t*)dstPixel + dword_offset) |= tmp;
        }
    } else { // float -> float
        uint32_t tmp;
        for (i = 0; i < 4; ++i) {
            if (0 == dstFrmt.GetChannelSize(i)) continue;

            Bits::CheckOffset(dstFrmt.GetChannelOffset(i), new_offset, dword_offset);

            tmp = *(uint32_t*)&srcPixel[i];
            assert(0 == new_offset);
            // tmp <<= new_offset; // new_offset must be always 0 (float size == 32)...

            *((uint32_t*)dstPixel + dword_offset) |= tmp;
        }
    }
}

void PackInts(void *dstPixel,
              const uint32_t *srcPixel,
              const ImageFormat &dstFrmt,
              uint32_t size)
{
    std::fill((uint8_t*)dstPixel, (uint8_t*)dstPixel + dstFrmt.GetPixelSize(), 0);

    uint32_t new_offset;
    uint32_t dword_offset;

    int i;
    if (dstFrmt.GetFlags() & ImageFormat::IntegerType) { // int -> int
        uint32_t tmp;
        for (i = 0; i < 4; ++i) {
            if (0 == dstFrmt.GetChannelSize(i)) continue;

            Bits::CheckOffset(dstFrmt.GetChannelOffset(i), new_offset, dword_offset);

            tmp = Bits::IntToInt(srcPixel[i], 0, size, new_offset, dstFrmt.GetChannelSize(i));

            *((uint32_t*)dstPixel + dword_offset) |= tmp;
        }
    } else if (dstFrmt.GetFlags() & ImageFormat::HalfFloatType) { // int -> float -> half float
        float f;
        uint32_t tmp;
        for (i = 0; i < 4; ++i) {
            if (0 == dstFrmt.GetChannelSize(i)) continue;

            f = Bits::IntToFloat(srcPixel[i], 0, size);
            tmp = *(uint32_t*)&f;
            tmp = Math::FloatToHalf(tmp);

            Bits::CheckOffset(dstFrmt.GetChannelOffset(i), new_offset, dword_offset);
            tmp <<= new_offset;

            *((uint32_t*)dstPixel + dword_offset) |= tmp;
        }
    } else { // int -> float
        float f;
        uint32_t tmp;
        for (i = 0; i < 4; ++i) {
            if (0 == dstFrmt.GetChannelSize(i)) continue;

            f = Bits::IntToFloat(srcPixel[i], 0, size);
            tmp = *(uint32_t*)&f;

            Bits::CheckOffset(dstFrmt.GetChannelOffset(i), new_offset, dword_offset);
            assert(0 == new_offset);
            // tmp <<= new_offset; // new_offset must be always 0 (float size == 32)...

            *((uint32_t*)dstPixel + dword_offset) |= tmp;
        }
    }
}

void UnpackFloats(const void *srcPixel,
                  float *dstPixel,
                  const ImageFormat &srcFrmt)
{
    uint32_t new_offset;
    uint32_t dword_offset;

    int i;
    if (srcFrmt.GetFlags() & ImageFormat::IntegerType) { // int -> float
        for (i = 0; i < 4; ++i) {
            if (0 == srcFrmt.GetChannelSize(i)) {
                dstPixel[i] = 1.0f;
                continue;
            }

            Bits::CheckOffset(srcFrmt.GetChannelOffset(i), new_offset, dword_offset);

            dstPixel[i] = Bits::IntToFloat(*(((uint32_t*)srcPixel) + dword_offset),
                                             new_offset,
                                             srcFrmt.GetChannelSize(i));
        }
    } else if (srcFrmt.GetFlags() & ImageFormat::HalfFloatType) { // half float -> float
        for (i = 0; i < 4; ++i) {
            if (0 == srcFrmt.GetChannelSize(i)) {
                dstPixel[i] = 1.0f;
                continue;
            }

            Bits::CheckOffset(srcFrmt.GetChannelOffset(i), new_offset, dword_offset);

            uint32_t tmp = *(((uint32_t*)srcPixel) + dword_offset);
            tmp &= Bits::ChannelMask(new_offset, srcFrmt.GetChannelSize(i));
            tmp >>= new_offset;
            tmp = Math::HalfToFloat((uint16_t)tmp);

            dstPixel[i] = *(float*)&tmp;
        }
    } else { // float -> float
        for (i = 0; i < 4; ++i) {
            if (0 == srcFrmt.GetChannelSize(i)) {
                dstPixel[i] = 1.0f;
                continue;
            }

            assert(32 == srcFrmt.GetChannelSize(i));
            Bits::CheckOffset(srcFrmt.GetChannelOffset(i), new_offset, dword_offset);
            assert(0 == new_offset);

            dstPixel[i] = *(((float*)srcPixel) + dword_offset);
        }
    }
}

void UnpackInts(const void *srcPixel,
                uint32_t *dstPixel,
                const ImageFormat &srcFrmt,
                uint32_t size)
{
    uint32_t new_offset;
    uint32_t dword_offset;

    int i;
    if (srcFrmt.GetFlags() & ImageFormat::IntegerType) { // int -> int
        for (i = 0; i < 4; ++i) {
            if (0 == srcFrmt.GetChannelSize(i)) {
                dstPixel[i] = Bits::ChannelMax(0, size);
                continue;
            }

            Bits::CheckOffset(srcFrmt.GetChannelOffset(i), new_offset, dword_offset);

            dstPixel[i] = Bits::IntToInt(*(((uint32_t*)srcPixel) + dword_offset),
                                           new_offset,
                                           srcFrmt.GetChannelSize(i),
                                           0,
                                           size);
        }
    } else if (srcFrmt.GetFlags() & ImageFormat::HalfFloatType) { // half float -> float -> int
        for (i = 0; i < 4; ++i) {
            if (0 == srcFrmt.GetChannelSize(i)) {
                dstPixel[i] = Bits::ChannelMax(0, size);
                continue;
            }

            Bits::CheckOffset(srcFrmt.GetChannelOffset(i), new_offset, dword_offset);

            uint32_t tmp = *(((uint32_t*)srcPixel) + dword_offset);
            tmp &= Bits::ChannelMask(new_offset, srcFrmt.GetChannelSize(i));
            tmp >>= new_offset;
            tmp = Math::HalfToFloat((uint16_t)tmp);

            dstPixel[i] = Bits::FloatToInt(*(float*)&tmp, 0, size);
        }
    } else { // float -> int
        for (i = 0; i < 4; ++i) {
            if (0 == srcFrmt.GetChannelSize(i)) {
                dstPixel[i] = Bits::ChannelMax(0, size);
                continue;
            }

            assert(32 == srcFrmt.GetChannelSize(i));
            Bits::CheckOffset(srcFrmt.GetChannelOffset(i), new_offset, dword_offset);
            assert(0 == new_offset);

            dstPixel[i] = Bits::FloatToInt(*(((float*)srcPixel) + dword_offset), 0, size);
        }
    }
}

void* GetPointer(uint32_t x, uint32_t y,
                 ImageFormat::Type format,
                 uint32_t w, uint32_t h,
                 void *pixels)
{
    return (void*)&((uint8_t*)pixels)[((y * w) + x) * ImageFormat(format).GetPixelSize()];
}

const void* GetPointer(uint32_t x, uint32_t y,
                       ImageFormat::Type format,
                       uint32_t w, uint32_t h,
                       const void *pixels)
{
    return (const void*)&((const uint8_t*)pixels)[((y * w) + x) * ImageFormat(format).GetPixelSize()];
}

    } // namespace Pixels
} // namespace Framework
