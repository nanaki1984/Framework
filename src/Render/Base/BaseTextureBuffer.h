#pragma once

#include "Core/EnumStrings.h"
#include "Render/Base/HardwareBuffer.h"
#include "Render/Image/ImageFormat.h"

namespace Framework {
    namespace RHI {

struct TextureBufferDesc;

class BaseTextureBuffer : public HardwareBuffer {
    DeclareClassInfo;
public:
    enum Type {
        Texture1D = 0,
        Texture2D,
        TextureCube,
        Texture3D
    };

    enum WrapMode {
        Clamp = 0,
        Repeat
    };

    enum FilterMode {
        Nearest = 0,
        Bilinear,
        Trilinear
    };
protected:
    uint32_t          width;
    uint32_t          height;
    uint32_t          depth;
    ImageFormat::Type format;
    Type              type;
    uint8_t           mipmapsRangeMin;
    uint8_t           mipmapsRangeMax;
    WrapMode          wrapMode;
    FilterMode        filterMode;
public:
    BaseTextureBuffer(const TextureBufferDesc &desc);
    virtual ~BaseTextureBuffer();

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetDepth() const;
    ImageFormat::Type GetFormat() const;
    Type GetType() const;
    void GetMipmapsRange(uint8_t &min, uint8_t &max) const;
    bool UsesMipmaps() const;

    WrapMode GetWrapMode() const;
    void SetWrapMode(WrapMode _wrapMode);

    FilterMode GetFilterMode() const;
    void SetFilterMode(FilterMode _filterMode);

    void Download(const LockInfo &lockInfo, void *data);
};

struct TextureBufferDesc {
    uint32_t                width;
    uint32_t                height;
    uint32_t                depth;
    ImageFormat::Type       format;
    BaseTextureBuffer::Type type;
    uint8_t                 mipmapsRangeMin;
    uint8_t                 mipmapsRangeMax;
    HardwareBuffer::Flags   flags;
};

uint8_t MipmapsRangeMax(uint32_t width, uint32_t height);
uint32_t MipmapSize(uint32_t size0, uint8_t level);

    } // namespace RHI
} // namespace Framework

DeclareEnumStrings(Framework::RHI::BaseTextureBuffer::Type, 4);
