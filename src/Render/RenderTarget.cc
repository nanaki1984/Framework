#include "Render/RenderTarget.h"
#include "Render/Resources/ResourceServer.h"
#include "Render/Resources/Texture.h"
#include "Render/RenderQueue.h"

namespace Framework {

DefineClassInfo(Framework::RenderTarget, Framework::RefCounted);

RenderTarget::RenderTarget(const RenderTargetDesc &desc)
: useMipmaps(desc.useMipmaps)
{
    RHI::TextureBufferDesc texDesc;

    texDesc.width = desc.width;
    texDesc.height = desc.height;
    texDesc.depth = 0;
    texDesc.format = desc.colorFormat;
    texDesc.type = desc.type;
    texDesc.flags = RHI::HardwareBuffer::RenderTarget;
    texDesc.mipmapsRangeMin = 0;
    texDesc.mipmapsRangeMax = useMipmaps ? RHI::MipmapsRangeMax(desc.width, desc.height) : 0;

    color = ResourceServer::Instance()->NewResource<Texture>(desc.name, Resource::ReadOnly);
    color->Load(texDesc);

    texDesc.type = RHI::BaseTextureBuffer::Texture2D;
    texDesc.format = desc.depthFormat;
    texDesc.mipmapsRangeMin = 0;
    texDesc.mipmapsRangeMax = 0;

    String depthName(desc.name.string);
    depthName.Append("-depth");

    depth = ResourceServer::Instance()->NewResource<Texture>(depthName, Resource::ReadOnly);
    depth->Load(texDesc);

    id = RenderQueue::Instance()->RegisterRenderTarget(this);
    RenderQueue::Instance()->GetRenderer()->OnRenderTargetCreated(this);
}

RenderTarget::~RenderTarget()
{
    RenderQueue::Instance()->GetRenderer()->OnRenderTargetDestroyed(this);
    RenderQueue::Instance()->UnregisterRenderTarget(id);

    ResourceServer::Instance()->DestroyResource(depth);
    ResourceServer::Instance()->DestroyResource(color);
}

const WeakPtr<Texture>&
RenderTarget::GetColor() const
{
    return color;
}

const WeakPtr<Texture>&
RenderTarget::GetDepth() const
{
    return depth;
}

} // namespace Framework
