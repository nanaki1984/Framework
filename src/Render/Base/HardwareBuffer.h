#pragma once

#include "Core/RefCounted.h"

namespace Framework {
    namespace RHI {

enum CubeFace {
    PositiveX = 0,
    NegativeX,
    PositiveY,
    NegativeY,
    PositiveZ,
    NegativeZ
};

union LockInfo {
    static const int32_t kGenerateMipmaps = -1;

    // VB/IB lock
    struct {
        uint32_t offset; // offset in bytes
        uint32_t size;   // size to lock in bytes
    };

    // Texture lock
    struct {
        uint16_t xOffset, yOffset; // left, top
        uint16_t width, height;    // width, height
        int32_t mipmapLevel;       // mipmap level, -1 to generate
        CubeFace cubeFace;         // cube face index
    };
};

class HardwareBuffer : public RefCounted {
    DeclareClassInfo;
public:
    enum Flags {
        NoFlags       = 0,
        Dynamic       = (1 << 0),
        RenderTarget  = (1 << 1),
        DepthStencil  = (1 << 2),
		ComputeBuffer = (1 << 3)
    };
protected:
    Flags    flags;
    uint32_t size;
public:
	HardwareBuffer(Flags _flags = NoFlags, uint32_t _size = 0);
	virtual ~HardwareBuffer();

	Flags GetFlags() const;
    uint32_t GetSize() const;

    void Upload(const LockInfo &lockInfo, const void *data);
};

    } // namespace RHI
} // namespace Framework
