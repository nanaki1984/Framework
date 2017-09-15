#include "Render/Base/BaseTextureBuffer.h"

namespace Framework {
    namespace RHI {

DefineClassInfo(Framework::RHI::BaseTextureBuffer, Framework::RHI::HardwareBuffer);

BaseTextureBuffer::BaseTextureBuffer(const TextureBufferDesc &desc)
: HardwareBuffer(desc.flags, 0),
  width(desc.width),
  height(desc.height),
  depth(desc.depth),
  format(desc.format),
  type(desc.type),
  mipmapsRangeMin(desc.mipmapsRangeMin),
  mipmapsRangeMax(desc.mipmapsRangeMax),
  wrapMode(Clamp),
  filterMode(Bilinear)
{
    ImageFormat imgFrmt = format;

    size = 0;
    for (uint8_t level = mipmapsRangeMin; level <= mipmapsRangeMax; ++level) {
        switch (type) {
            case Texture1D:
                size += imgFrmt.GetSurfaceSize(MipmapSize(width, level), 1);
                break;
            case Texture2D:
                size += imgFrmt.GetSurfaceSize(MipmapSize(width, level), MipmapSize(height, level));
                break;
            case TextureCube:
                size += 6 * imgFrmt.GetSurfaceSize(MipmapSize(width, level), MipmapSize(height, level));
                break;
            case Texture3D:
                assert(false);
                break;
        }
    }
}

BaseTextureBuffer::~BaseTextureBuffer ( )
{ }

uint32_t
BaseTextureBuffer::GetWidth() const
{
    return width;
}

uint32_t
BaseTextureBuffer::GetHeight() const
{
    return height;
}

uint32_t
BaseTextureBuffer::GetDepth() const
{
    return depth;
}

ImageFormat::Type
BaseTextureBuffer::GetFormat() const
{
    return format;
}

BaseTextureBuffer::Type
BaseTextureBuffer::GetType() const
{
    return type;
}

void
BaseTextureBuffer::GetMipmapsRange(uint8_t &min, uint8_t &max) const
{
    min = mipmapsRangeMin;
    max = mipmapsRangeMax;
}

bool
BaseTextureBuffer::UsesMipmaps() const
{
    return mipmapsRangeMin || mipmapsRangeMax;
}

BaseTextureBuffer::WrapMode
BaseTextureBuffer::GetWrapMode() const
{
    return wrapMode;
}

void
BaseTextureBuffer::SetWrapMode(WrapMode _wrapMode)
{
    wrapMode = _wrapMode;
}

BaseTextureBuffer::FilterMode
BaseTextureBuffer::GetFilterMode() const
{
    return filterMode;
}

void
BaseTextureBuffer::SetFilterMode(FilterMode _filterMode)
{
    filterMode = _filterMode;
}

void
BaseTextureBuffer::Download(const LockInfo &lockInfo, void *data)
{
    assert(lockInfo.mipmapLevel >= mipmapsRangeMin && lockInfo.mipmapLevel <= mipmapsRangeMax);
}

uint8_t MipmapsRangeMax(uint32_t width, uint32_t height)
{
    uint32_t size = (width >= height ? width : height);
    uint8_t layer = 0;
    while ((size >>= 1) > 0)
        ++layer;
    return layer;
}

uint32_t MipmapSize(uint32_t size0, uint8_t level)
{
    uint32_t s = size0 >> level;
    return 0 == s ? 1 : s;
}

    } // namespace RHI
} // namespace Framework

DefineEnumStrings(Framework::RHI::BaseTextureBuffer::Type) = {
    "Texture1D",
    "Texture2D",
    "TextureCube",
    "Texture3D"
};
