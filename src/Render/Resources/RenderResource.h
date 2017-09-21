#pragma once

#include "Render/Resources/Resource.h"
#include "Core/Collections/List.h"

namespace Framework {
    namespace RHI {

struct RenderData // Noncopyable
{
    RenderData() { }

    RenderData(const RenderData &) = delete;
    RenderData& operator = (const RenderData &) = delete;

    void Invalidate() { }
};

    } // namespace RHI

template <typename RHIRenderData>
class RenderResource : public Resource
{
protected:
    RHIRenderData renderData;
    RHIRenderData clientRenderData;
public:
    RenderResource();
    virtual ~RenderResource();

    virtual void OnEndFrameCommands() override;

    const RHIRenderData& GetRenderData() const;
};

template<typename RHIRenderData>
RenderResource<RHIRenderData>::RenderResource()
{ }

template<typename RHIRenderData>
RenderResource<RHIRenderData>::~RenderResource()
{
    renderData.Invalidate();
    clientRenderData.Invalidate();
}

template<typename RHIRenderData>
const RHIRenderData&
RenderResource<RHIRenderData>::GetRenderData() const
{
    return renderData;
}

template<typename RHIRenderData>
void
RenderResource<RHIRenderData>::OnEndFrameCommands()
{
    renderData.Invalidate();

    std::swap(renderData, clientRenderData);

    clientRenderData.Invalidate();
}

} // namespace Framework
