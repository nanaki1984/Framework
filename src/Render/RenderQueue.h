#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include "Core/Singleton.h"
#include "Render/MaterialParamsBlock.h"
#include "Render/Renderer.h"
#include "Core/Collections/SimplePool_type.h"
#include "Render/KeyCode.h"

namespace Framework {

class RenderQueue : public Singleton<RenderQueue> {
    DeclareClassInfo;
protected:
    SmartPtr<RHI::Renderer> renderer;

    SimplePool<MaterialParamsBlock> paramsBlocks;
    SimplePool<MaterialParamsBlock> clientParamsBlocks;

    Array<RHI::KeyCode> commands;
	Array<RHI::KeyCode> clientCommands;

    Resource::IntrusiveList resourcesList;

    SimplePool<RenderTarget*> renderTargets;
    
    uint32_t frameCount;

    std::mutex              rtMutex;
	std::condition_variable rtSignal;
	std::thread             renderThread;

    bool newFrame, frameCompleted, quitRequest;

    bool inBeginFrameCmds;

	void RenderFrames();
public:
    RenderQueue();
    virtual ~RenderQueue();

    const SmartPtr<RHI::Renderer>& GetRenderer() const;

    uint32_t GetMaterialParamsBlock(MaterialParamsBlock **outPointer);

    void BeginFrameCommands();
    void RegisterResource(Resource *resource);
    void SendCommand(RHI::KeyCode command);
    void EndFrameCommands();

    uint32_t RegisterRenderTarget(RenderTarget *rt);
    void UnregisterRenderTarget(uint32_t id);

    uint32_t GetFrameCount() const;

    bool IsRenderThread() const;
};

inline const SmartPtr<RHI::Renderer>&
RenderQueue::GetRenderer() const
{
    return renderer;
}

inline uint32_t
RenderQueue::GetFrameCount() const
{
    return frameCount;
}

inline bool
RenderQueue::IsRenderThread() const
{
    return std::this_thread::get_id() == renderThread.get_id();
}

} // namespace Render
