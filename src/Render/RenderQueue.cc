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
  renderTargets(Memory::GetAllocator<MallocAllocator>()),
  commands(Memory::GetAllocator<MallocAllocator>()),
  commandsCopy(Memory::GetAllocator<MallocAllocator>()),
  renderThread(&RenderQueue::RenderFrame, this),
  newFrame(false),
  frameCompleted(true),
  inBeginFrameCmds(false)
{
    renderer = SmartPtr<RHI::Renderer>::MakeNew<MallocAllocator>();
}

RenderQueue::~RenderQueue()
{ }

uint32_t
RenderQueue::GetMaterialParamsBlock(MaterialParamsBlock **outPointer)
{
	paramsLock.lock();
    uint32_t index = paramsBlocks.Allocate();
	paramsLock.unlock();
    *outPointer = paramsBlocks.Begin() + index;
    (*outPointer)->Clear();
    return index;
}

void
RenderQueue::BeginFrameCommands()
{
	assert(!inBeginFrameCmds);
    inBeginFrameCmds = true;
}

void
RenderQueue::SendCommand(RHI::KeyCode command)
{
	assert(inBeginFrameCmds);
	commandsCopy.PushBack(command);
}

void
RenderQueue::EndFrameCommands()
{
	assert(inBeginFrameCmds);
	inBeginFrameCmds = false;

	{
		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, [this] { return frameCompleted; });
	}
	RefCounted::GC.Collect();
	
	commands.Clear();
	commands.InsertRange(0, commandsCopy.Begin(), commandsCopy.Count());
	commandsCopy.Clear(); // ToDo: swap
	
	{
		std::lock_guard<std::mutex> lk(m);
		newFrame = true;
	}
	cv.notify_one();

	//commands.Clear();
	//commands.InsertRange(0, commandsCopy.Begin(), commandsCopy.Count());
	//commandsCopy.Clear();
	//this->RenderFrame();
	//RefCounted::GC.Collect();
}

void
RenderQueue::RenderFrame()
{
	GLFWwindow *win = Application::Instance()->GetRenderThreadWindow();
	glfwMakeContextCurrent(win);
	//glewInit();

	for (;;) {
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk, [this] { return newFrame; });
	newFrame = frameCompleted = false;

	float t0 = TimeServer::Instance()->GetMilliseconds();

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
					paramsLock.lock();
					const MaterialParamsBlock &params = paramsBlocks.Get(key.GetDispatchParamsBlockId());
					paramsLock.unlock();
					renderer->Dispatch(key.GetComputeShaderId(), x, y, z, params);
					paramsLock.lock();
					paramsBlocks.Free(key.GetDispatchParamsBlockId());
					paramsLock.unlock();
					break;
                }
                case RHI::Key::kCommandDrawCall:
					if (renderer->SetMaterialPass(key.GetMaterialId(), key.GetMaterialPass()))
					{
						paramsLock.lock();
						const MaterialParamsBlock &params = paramsBlocks.Get(key.GetParamsBlockId());
						paramsLock.unlock();
						renderer->DrawMesh(key.GetMeshId(), key.GetSubMeshIndex(), params);
					}
					paramsLock.lock();
					paramsBlocks.Free(key.GetParamsBlockId());
					paramsLock.unlock();
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }

    //ImGui::Render();

    renderer->EndFrame();

	glfwSwapBuffers(win);

	float t1 = TimeServer::Instance()->GetMilliseconds();
    Log::Instance()->Write(Log::Info, "Rendering time: %f (dc: %d)", (t1 - t0), renderer->GetDrawCallsCount());

	frameCompleted = true;
	lk.unlock();
	cv.notify_one();
	} // for(;;)

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
