#pragma once

#include "Core/Singleton.h"
#include "Render/RenderObjects.h"
#include "Render/Renderer.h"
#include "Render/MaterialParamsBlock.h"
#include "Core/Collections/SimplePool_type.h"
#include "Render/KeyCode.h"

namespace Framework {

class RenderQueue : public Singleton<RenderQueue> {
    DeclareClassInfo;
protected:
    struct Frame {
        uint32_t startIndex;
        uint32_t endIndex;
    };

    SmartPtr<RHI::Renderer> renderer;

    SimplePool<MaterialParamsBlock> paramsBlocks;
    SimplePool<RenderTarget*> renderTargets;

    Array<RHI::KeyCode> commands;
    Queue<Frame> frames;

    bool inBeginFrameCmds;
public:
    RenderQueue();
    virtual ~RenderQueue();

    const SmartPtr<RHI::Renderer>& GetRenderer() const;

    uint32_t GetMaterialParamsBlock(MaterialParamsBlock **outPointer);

    void BeginFrameCommands();
    void SendCommand(RHI::KeyCode command);
    void EndFrameCommands();

    void RenderFrame();

    uint32_t RegisterRenderTarget(RenderTarget *rt);
    void UnregisterRenderTarget(uint32_t id);
};

inline const SmartPtr<RHI::Renderer>&
RenderQueue::GetRenderer() const
{
    return renderer;
}

} // namespace Render
