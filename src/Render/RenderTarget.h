#pragma once

#include "Core/RefCounted.h"
#include "Core/WeakPtr.h"
#include "Core/StringHash.h"
#include "Render/Base/BaseTextureBuffer.h"

namespace Framework {

class Texture;

struct RenderTargetDesc {
    StringHash name;

    uint32_t width;
    uint32_t height;

    RHI::BaseTextureBuffer::Type type;

    ImageFormat::Type colorFormat;
    ImageFormat::Type depthFormat;

    bool useMipmaps;
};

class RenderTarget : public RefCounted {
    DeclareClassInfo;
protected:
    WeakPtr<Texture> color;
    WeakPtr<Texture> depth;

    bool useMipmaps;

    uint32_t id;
public:
    RenderTarget(const RenderTargetDesc &desc);
    virtual ~RenderTarget();

    const WeakPtr<Texture>& GetColor() const;
    const WeakPtr<Texture>& GetDepth() const;

    uint32_t GetId() const;
};

inline uint32_t
RenderTarget::GetId() const
{
    return id;
}

} // namespace Framework
