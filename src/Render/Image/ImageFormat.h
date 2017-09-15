#pragma once

#include <cstdint>
#include "Core/Debug.h"

namespace Framework {

class ImageFormat {
public:
    enum Type {
        InvalidFormat = 0,
        
        // image formats
        L8,             //  8 bits
        A8L8,           // 16 bits
        R5G6B5,         // 16 bits
        A1R5G5B5,       // 16 bits
        A4R4G4B4,       // 16 bits
        L16,            // 16 bits
        R16F,           // 16 bits
        A2B10G10R10,    // 32 bits
        G16R16,         // 32 bits
        X8R8G8B8,       // 32 bits
        A8R8G8B8,       // 32 bits
        G16R16F,        // 32 bits
        R32F,           // 32 bits
        G32R32F,        // 64 bits
        A16B16G16R16F,  // 64 bits
        A32B32G32R32F,  //128 bits

        // compressed image formats
        DXT1_RGB,
        DXT3_RGBA,
        DXT5_RGBA,

        // depth stencil formats
        D16,            // 16 bits, no stencil
        D15S1,          // 16 bits
        D32,            // 32 bits, no stencil
        D24X8,          // 32 bits, no stencil
        D24S8,          // 32 bits

        NumFormats
    };

    enum Flags {
        IntegerType   = (1 << 1),
        HalfFloatType = (1 << 2),
        FloatType     = (1 << 3),
        HasAlpha      = (1 << 4),
        HasLuminance  = (1 << 5),
        Compressed    = (1 << 6)
    };
private:
    struct ChannelStruct {
        uint8_t offset;
        uint8_t size;
    };

    struct DataStruct {
        ChannelStruct channels[4];
        Flags         flags;
        uint16_t      pixelSize;
        uint16_t      blockSize;
    };

    Type        type;
    DataStruct* data;

    static DataStruct DataFormats[];
public:
    ImageFormat(Type t = InvalidFormat);
    
    Type GetType() const;
    uint8_t GetChannelOffset(int channelIndex) const;
    uint8_t GetChannelSize(int channelIndex) const;
    Flags GetFlags() const;
    uint16_t GetPixelSize() const;
    uint16_t GetBlockSize() const;
    uint32_t GetSurfaceSize(uint32_t width, uint32_t height) const;
};

inline ImageFormat::Type
ImageFormat::GetType() const
{
    return type;
}

inline uint8_t
ImageFormat::GetChannelOffset(int channelIndex) const
{
    assert(channelIndex >= 0 && channelIndex < 4);
    return data->channels[channelIndex].offset;
}

inline uint8_t
ImageFormat::GetChannelSize(int channelIndex) const
{
    assert(channelIndex >= 0 && channelIndex < 4);
    return data->channels[channelIndex].size;
}

inline ImageFormat::Flags
ImageFormat::GetFlags() const
{
    return data->flags;
}

inline uint16_t
ImageFormat::GetPixelSize() const
{
    return data->pixelSize;
}

inline uint16_t
ImageFormat::GetBlockSize() const
{
    assert(data->flags & Compressed);
    return data->blockSize;
}

inline uint32_t
ImageFormat::GetSurfaceSize(uint32_t width, uint32_t height) const
{
    if (data->flags & Compressed) {
        return (((width + 3) & ~3) * ((height + 3) & ~3) * data->blockSize) / 16;
    } else {
        return width * height * data->pixelSize;
    }
}

} // namespace Framework
