#pragma once

#include <cstdint>
#include "Render/Image/ImageFormat.h"

namespace Framework {
    namespace Pixels {

void PackInts(void *dstPixel,
              const uint32_t *srcPixel,
              const ImageFormat &dstFrmt,
              uint32_t size = 16);
void UnpackInts(const void *srcPixel,
                uint32_t *dstPixel,
                const ImageFormat &srcFrmt,
                uint32_t size = 16);

void PackFloats(void *dstPixel,
                const float *srcPixel,
                const ImageFormat &dstFrmt);
void UnpackFloats(const void *srcPixel,
                  float *dstPixel,
                  const ImageFormat &srcFrmt);

void* GetPointer(uint32_t x, uint32_t y,
                 ImageFormat::Type format,
                 uint32_t w, uint32_t h,
                 void *pixels);
const void* GetPointer(uint32_t x, uint32_t y,
                       ImageFormat::Type format,
                       uint32_t w, uint32_t h,
                       const void *pixels);

    } // namespace Pixels
} // namespace Framework
