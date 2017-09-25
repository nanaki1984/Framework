#include "Render/RenderQueue.h"
#include "Core/Collections/SimplePool.h"
#include "Render/Key.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Time/TimeServer.h"
#include "Core/Log.h"
#include "imgui.h"

#include "Core/Application.h"
#include "GLFW/glfw3.h"

namespace Framework {

DefineClassInfo(Framework::RenderQueue, Framework::RefCounted);

RenderQueue::RenderQueue()
: paramsBlocks(Memory::GetAllocator<MallocAllocator>()),
  clientParamsBlocks(Memory::GetAllocator<MallocAllocator>()),
  commands(Memory::GetAllocator<MallocAllocator>()),
  clientCommands(Memory::GetAllocator<MallocAllocator>()),
  renderTargets(Memory::GetAllocator<MallocAllocator>()),
  frameCount(0),
  renderThread(&RenderQueue::RenderFrames, this),
  newFrame(false),
  frameCompleted(true),
  quitRequest(false),
  inBeginFrameCmds(false)
{
    renderer = SmartPtr<RHI::Renderer>::MakeNew<MallocAllocator>();
}

RenderQueue::~RenderQueue()
{
    {
        std::unique_lock<std::mutex> lock(rtMutex);
        while (!frameCompleted)
            rtSignal.wait(lock);
    }

    renderer->OnEndFrameCommands();

    resourcesList.Clear();

    {
        std::lock_guard<std::mutex> guard(rtMutex);
        quitRequest = true;
    }
    rtSignal.notify_one();

    renderThread.join();
}

uint32_t
RenderQueue::GetMaterialParamsBlock(MaterialParamsBlock **outPointer)
{
    uint32_t index = clientParamsBlocks.Allocate();
    *outPointer = clientParamsBlocks.Begin() + index;
    (*outPointer)->Clear();
    return index;
}

void
RenderQueue::BeginFrameCommands()
{
	assert(!inBeginFrameCmds);
    inBeginFrameCmds = true;
    resourcesList.Clear();
}

void
RenderQueue::RegisterResource(Resource *resource)
{
    assert(inBeginFrameCmds);
    resourcesList.PushBack(resource);
}

void
RenderQueue::SendCommand(RHI::KeyCode command)
{
	assert(inBeginFrameCmds);
    clientCommands.PushBack(command);
}

void
RenderQueue::EndFrameCommands()
{
	assert(inBeginFrameCmds);
	inBeginFrameCmds = false;

	{
		std::unique_lock<std::mutex> lock(rtMutex);
        while (!frameCompleted)
            rtSignal.wait(lock);
	}

    auto *res = resourcesList.Begin();
    while (res != nullptr)
    {
        res->OnEndFrameCommands();

        res = resourcesList.GetNext(res);
    }

    renderer->OnEndFrameCommands();

    RefCounted::GC.Collect();

    ++frameCount;

    std::swap(commands, clientCommands);
    std::swap(paramsBlocks, clientParamsBlocks);
	
    assert(0 == clientCommands.Count());
    assert(0 == clientParamsBlocks.Count());

    {
		std::lock_guard<std::mutex> guard(rtMutex);
		newFrame = true;
	}
	rtSignal.notify_one();
}

void
RenderQueue::RenderFrames()
{
	GLFWwindow *ctx = Application::Instance()->GetRenderingContext();
	glfwMakeContextCurrent(ctx);
    glfwSwapInterval(0);
    glewInit();

	for (;;)
    {
	    std::unique_lock<std::mutex> lock(rtMutex);
        while (!newFrame && !quitRequest)
            rtSignal.wait(lock);

        if (quitRequest)
            break;

        newFrame = frameCompleted = false;

        renderer->BeginFrame();

        uint32_t cmdsCount = commands.Count();
        if (cmdsCount > 0)
        {
            Array<RHI::KeyCode>::Sort(commands, 0, cmdsCount, [](const RHI::KeyCode &a, const RHI::KeyCode &b) {
                return memcmp(a.bytes, b.bytes, 14) < 0;//sizeof(KeyCode)
            });

            for (uint32_t i = 0; i < cmdsCount; ++i) {
                RHI::Key key = commands[i];
                switch (key.GetCommand()) {
                    case RHI::Key::kCommandSetRenderTarget:
                        if (RHI::Key::kResetRenderTarget == key.GetRenderTargetId())
                            renderer->ResetRenderTarget();
                        else
                            renderer->SetRenderTarget(renderTargets.Get(key.GetRenderTargetId()), key.GetRenderTargetCubeface());
                        break;
                    case RHI::Key::kCommandSetViewport:
                        renderer->SetViewport(key.GetViewport());
                        break;
                    case RHI::Key::kCommandClear:
                        renderer->Clear(key.GetClearFlags(), key.GetClearColor(), key.GetClearDepth(), key.GetClearStencil());
                        break;
                    case RHI::Key::kCommandDispatch:
                    {
                        int x, y, z;
                        key.GetNumWorkGroups(x, y, z);
					    const MaterialParamsBlock &params = paramsBlocks.Get(key.GetDispatchParamsBlockId());
					    renderer->Dispatch(key.GetComputeShaderId(), x, y, z, params);
					    paramsBlocks.Free(key.GetDispatchParamsBlockId());
					    break;
                    }
                    case RHI::Key::kCommandDrawCall:
					    if (renderer->SetMaterialPass(key.GetMaterialId(), key.GetMaterialPass()))
					    {
						    const MaterialParamsBlock &params = paramsBlocks.Get(key.GetParamsBlockId());
						    renderer->DrawMesh(key.GetMeshId(), key.GetSubMeshIndex(), params);
					    }
					    paramsBlocks.Free(key.GetParamsBlockId());
                        break;
                    default:
                        assert(false);
                        break;
                }
            }

            commands.Clear();
        }

        renderer->EndFrame();

	    glfwSwapBuffers(ctx);

	    frameCompleted = true;

        lock.unlock();
	    rtSignal.notify_one();
	}

	glfwMakeContextCurrent(nullptr);
}

uint32_t
RenderQueue::RegisterRenderTarget(RenderTarget *rt)
{
    uint32_t index = renderTargets.Allocate();
    renderTargets.Get(index) = rt;
    return index;
}

void
RenderQueue::UnregisterRenderTarget(uint32_t id)
{
    renderTargets.Free(id);
}

} // namespace Render
