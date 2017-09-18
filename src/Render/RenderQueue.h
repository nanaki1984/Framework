#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

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
    SimplePool<MaterialParamsBlock> clientParamsBlocks;

    Array<RHI::KeyCode> commands;
	Array<RHI::KeyCode> clientCommands;

    SimplePool<RenderTarget*> renderTargets;
    
    std::mutex m;
	std::condition_variable cv;
	std::thread renderThread;
	bool newFrame, frameCompleted;

    bool inBeginFrameCmds;

	void RenderFrame();
public:
    RenderQueue();
    virtual ~RenderQueue();

    const SmartPtr<RHI::Renderer>& GetRenderer() const;

    uint32_t GetMaterialParamsBlock(MaterialParamsBlock **outPointer);

    void BeginFrameCommands();
    void SendCommand(RHI::KeyCode command);
    void EndFrameCommands();

    uint32_t RegisterRenderTarget(RenderTarget *rt);
    void UnregisterRenderTarget(uint32_t id);

    bool IsRenderThread() const;
};

inline const SmartPtr<RHI::Renderer>&
RenderQueue::GetRenderer() const
{
    return renderer;
}

inline bool
RenderQueue::IsRenderThread() const
{
    return std::this_thread::get_id() == renderThread.get_id();
}

} // namespace Render
