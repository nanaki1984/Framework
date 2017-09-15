#include "Render/Image/Image.h"
#include "Render/Image/Pixels.h"
#include "Render/Utils/Color.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"

namespace Framework {

Image::Image()
: allocator(nullptr),
  format(ImageFormat::InvalidFormat),
  width(0),
  height(0),
  pixels(nullptr)
{ }

Image::Image(Allocator &_allocator, ImageFormat::Type _format, uint32_t _width, uint32_t _height)
: allocator(nullptr),
  format(ImageFormat::InvalidFormat),
  width(0),
  height(0),
  pixels(nullptr)
{
    this->Allocate(_allocator, _format, _width, _height);
}

Image::Image(const Image &other)
: allocator(nullptr),
  format(ImageFormat::InvalidFormat),
  width(0),
  height(0),
  pixels(nullptr)
{
    this->Allocate(*other.allocator, other.format, other.width, other.height);
    memcpy(pixels, other.pixels, ImageFormat(format).GetSurfaceSize(width, height));
}

Image::Image(Image &&other)
: allocator(other.allocator),
  format(other.format),
  width(other.width),
  height(other.height),
  pixels(other.pixels)
{
    other.allocator = nullptr;
    other.format = ImageFormat::InvalidFormat;
    other.width = other.height = 0;
    other.pixels = nullptr;
}

Image::~Image()
{
    this->Deallocate();
}

Image&
Image::operator =(const Image &other)
{
    this->Allocate(*other.allocator, other.format, other.width, other.height);
    memcpy(pixels, other.pixels, ImageFormat(format).GetSurfaceSize(width, height));
    return (*this);
}

Image&
Image::operator =(Image &&other)
{
    allocator = other.allocator;
    format = other.format;
    width = other.width;
    height = other.height;
    pixels = other.pixels;

    other.allocator = nullptr;
    other.format = ImageFormat::InvalidFormat;
    other.width = other.height = 0;
    other.pixels = nullptr;

    return (*this);
}

void
Image::Allocate(Allocator &_allocator, ImageFormat::Type _format, uint32_t _width, uint32_t _height)
{
    this->Deallocate();

    if (_format == ImageFormat::InvalidFormat || 0 == _width || 0 == _height)
        return;

    allocator = &_allocator;
    format = _format;
    width = _width;
    height = _height;
    pixels = allocator->Allocate(ImageFormat(format).GetSurfaceSize(width, height), 1);
}

void
Image::Deallocate()
{
    if (nullptr == pixels)
        return;

    allocator->Free(pixels);

    allocator = nullptr;
    format = ImageFormat::InvalidFormat;
    width = 0;
    height = 0;
    pixels = nullptr;
}

void
Image::SetPixels(ImageFormat::Type srcFormat, const void *srcPixels, uint32_t count, uint32_t offset)
{
    assert(0 == (ImageFormat(format).GetFlags() & ImageFormat::Compressed));
    if (nullptr == pixels || srcFormat == ImageFormat::InvalidFormat || nullptr == srcPixels)
        return;

    ImageFormat srcFrmt(srcFormat),
                dstFrmt(format);

    const uint8_t *srcPixel = static_cast<const uint8_t*>(srcPixels);
    uint8_t       *dstPixel = static_cast<uint8_t*>(pixels) + dstFrmt.GetPixelSize() * offset;

    for (uint32_t i = 0; i < count; ++i) {
        if (srcFrmt.GetFlags() & ImageFormat::FloatType || srcFrmt.GetFlags() & ImageFormat::HalfFloatType) {
            float cf[4];

            Pixels::UnpackFloats(srcPixel, cf, srcFrmt);
            Pixels::PackFloats(dstPixel, cf, dstFrmt);
        } else {
            uint32_t ci[4];

            Pixels::UnpackInts(srcPixel, ci, srcFrmt);
            Pixels::PackInts(dstPixel, ci, dstFrmt);
        }

        srcPixel += srcFrmt.GetPixelSize();
        dstPixel += dstFrmt.GetPixelSize();
    }
}

Color
Image::GetPixel(uint32_t x, uint32_t y) const
{
    assert(0 == (ImageFormat(format).GetFlags() & ImageFormat::Compressed));
    if (nullptr == pixels || ImageFormat::InvalidFormat == format)
        return Color::Black;

    Color c;
    const void *pixel = Pixels::GetPointer(x, y, format, width, height, pixels);
    Pixels::UnpackFloats(pixel, c.rgba, ImageFormat(format));
    return c;
}

void
Image::SetPixel(uint32_t x, uint32_t y, const Color &c)
{
    assert(0 == (ImageFormat(format).GetFlags() & ImageFormat::Compressed));
    if (nullptr == pixels || ImageFormat::InvalidFormat == format)
        return;

    void *pixel = Pixels::GetPointer(x, y, format, width, height, pixels);
    Pixels::PackFloats(pixel, c.rgba, ImageFormat(format));
}

void
Image::GetPixels(Color *dstPixels, uint32_t count, uint32_t offset) const
{
    assert(0 == (ImageFormat(format).GetFlags() & ImageFormat::Compressed));
    if (nullptr == pixels || ImageFormat::InvalidFormat == format || nullptr == dstPixels)
        return;

    ImageFormat srcFrmt(format);

    const uint8_t *srcPixel = static_cast<const uint8_t*>(pixels) + srcFrmt.GetPixelSize() * offset;

    for (uint32_t i = 0; i < count; ++i) {
        Pixels::UnpackFloats(srcPixel, dstPixels[i].rgba, srcFrmt);

        srcPixel += srcFrmt.GetPixelSize();
    }
}

void
Image::SetPixels(const Color *srcPixels, uint32_t count, uint32_t offset)
{
    assert(0 == (ImageFormat(format).GetFlags() & ImageFormat::Compressed));
    if (nullptr == pixels || ImageFormat::InvalidFormat == format || nullptr == srcPixels)
        return;

    ImageFormat dstFrmt(format);

    uint8_t *dstPixel = static_cast<uint8_t*>(pixels) + dstFrmt.GetPixelSize() * offset;

    for (uint32_t i = 0; i < count; ++i) {
        Pixels::PackFloats(dstPixel, srcPixels[i].rgba, dstFrmt);

        dstPixel += dstFrmt.GetPixelSize();
    }
}

void
Image::Convert(ImageFormat::Type newFormat)
{
    assert(0 == (ImageFormat(format).GetFlags() & ImageFormat::Compressed));
    if (nullptr == pixels || format == newFormat)
        return;

    ImageFormat srcFrmt(format),
                dstFrmt(newFormat);

    void *newPixels = allocator->Allocate(width * height * dstFrmt.GetPixelSize(), 1);

    for (uint32_t x = 0; x < width; ++x) {
        for (uint32_t y = 0; y < height; ++y) {
            const void *srcPixel = Pixels::GetPointer(x, y,    format, width, height,    pixels);
            void       *dstPixel = Pixels::GetPointer(x, y, newFormat, width, height, newPixels);

            if (srcFrmt.GetFlags() & ImageFormat::FloatType || srcFrmt.GetFlags() & ImageFormat::HalfFloatType) {
                float cf[4];

                Pixels::UnpackFloats(srcPixel, cf, srcFrmt);
                Pixels::PackFloats(dstPixel, cf, dstFrmt);
            } else {
                uint32_t ci[4];

                Pixels::UnpackInts(srcPixel, ci, srcFrmt);
                Pixels::PackInts(dstPixel, ci, dstFrmt);
            }
        }
    }

    allocator->Free(pixels);

    format = newFormat;
    pixels = newPixels;
}

} // namespace Framework
