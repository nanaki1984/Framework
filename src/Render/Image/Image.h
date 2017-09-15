#pragma once

#include "Core/RefCounted.h"
#include "Render/Image/ImageFormat.h"

namespace Framework {

class Allocator;
class Color;

class Image {
protected:
    Allocator         *allocator;
    ImageFormat::Type format;
    uint32_t          width;
    uint32_t          height;
    void*             pixels;
public:
    Image();
    Image(Allocator &_allocator, ImageFormat::Type _format, uint32_t _width, uint32_t _height);
    Image(const Image &other);
    Image(Image &&other);
    ~Image();

    Image& operator =(const Image &other);
    Image& operator =(Image &&other);

    void Allocate(Allocator &_allocator, ImageFormat::Type frmt, uint32_t w, uint32_t h);
    void Deallocate();
    bool IsAllocated() const;

    ImageFormat::Type GetFormat() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetSize() const;

    const void* GetPixels() const;
    void* GetPixels();
    void SetPixels(ImageFormat::Type srcFormat, const void* srcPixels, uint32_t count, uint32_t offset = 0);

    Color GetPixel(uint32_t x, uint32_t y) const;
    void SetPixel(uint32_t x, uint32_t y, const Color &c);
    void GetPixels(Color *dstPixels, uint32_t count, uint32_t offset = 0) const;
    void SetPixels(const Color *srcPixels, uint32_t count, uint32_t offset = 0);

    void Convert(ImageFormat::Type newFormat);
};

inline bool
Image::IsAllocated() const
{
    return allocator != nullptr;
}

inline ImageFormat::Type
Image::GetFormat() const
{
    return format;
}

inline uint32_t
Image::GetWidth() const
{
    return width;
}

inline uint32_t
Image::GetHeight() const
{
    return height;
}

inline uint32_t
Image::GetSize() const
{
    return ImageFormat(format).GetSurfaceSize(width, height);
}

inline const void*
Image::GetPixels() const
{
    return pixels;
}

inline void*
Image::GetPixels()
{
    return pixels;
}

} // namespace Framework
