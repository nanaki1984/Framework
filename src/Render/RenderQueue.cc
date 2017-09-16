#include "Render/RenderQueue.h"
#include "Core/Collections/SimplePool.h"
#include "Render/Key.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Time/TimeServer.h"
#include "Core/Log.h"
#include "imgui.h"

namespace Framework {

DefineClassInfo(Framework::RenderQueue, Framework::RefCounted);

RenderQueue::RenderQueue()
: paramsBlocks(Memory::GetAllocator<MallocAllocator>()),
  renderTargets(Memory::GetAllocator<MallocAllocator>()),
  commands(Memory::GetAllocator<MallocAllocator>()),
  frames(Memory::GetAllocator<MallocAllocator>()),
  inBeginFrameCmds(false)
{
    renderer = SmartPtr<RHI::Renderer>::MakeNew<MallocAllocator>();
}

RenderQueue::~RenderQueue()
{ }

uint32_t
RenderQueue::GetMaterialParamsBlock(MaterialParamsBlock **outPointer)
{
    uint32_t index = paramsBlocks.Allocate();
    *outPointer = paramsBlocks.Begin() + index;
    (*outPointer)->Clear();
    return index;
}

void
RenderQueue::BeginFrameCommands()
{
	assert(!inBeginFrameCmds);
    inBeginFrameCmds = true;

    frames.PushBack(Frame());
    frames.Back().startIndex = commands.Count();
}

void
RenderQueue::SendCommand(RHI::KeyCode command)
{
	assert(inBeginFrameCmds);
    commands.PushBack(command);
}

void
RenderQueue::EndFrameCommands()
{
    frames.Back().endIndex = commands.Count();

	assert(inBeginFrameCmds);
	inBeginFrameCmds = false;
}

void
RenderQueue::RenderFrame()
{
	if (inBeginFrameCmds || frames.IsEmpty())
        return;

	float t0 = TimeServer::Instance()->GetMilliseconds();

	Frame frame = frames.Front();
    frames.PopFront();

    renderer->BeginFrame();

    uint32_t cmdsCount = frame.endIndex - frame.startIndex;
    if (cmdsCount > 0)
    {
        Array<RHI::KeyCode>::Sort(commands, frame.startIndex, cmdsCount, [](const RHI::KeyCode &a, const RHI::KeyCode &b) {
            return memcmp(a.bytes, b.bytes, 14) < 0;//sizeof(KeyCode)
        });

        for (uint32_t i = frame.startIndex; i < frame.endIndex; ++i) {
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
                    renderer->Dispatch(key.GetComputeShaderId(), x, y, z, paramsBlocks.Get(key.GetDispatchParamsBlockId()));
                    paramsBlocks.Free(key.GetDispatchParamsBlockId());
                    break;
                }
                case RHI::Key::kCommandDrawCall:
                    if (renderer->SetMaterialPass(key.GetMaterialId(), key.GetMaterialPass()))
                        renderer->DrawMesh(key.GetMeshId(), key.GetSubMeshIndex(), paramsBlocks.Get(key.GetParamsBlockId()));
                    paramsBlocks.Free(key.GetParamsBlockId());
                    break;
                default:
                    assert(false);
                    break;
            }
        }

        commands.RemoveRange(frame.startIndex, cmdsCount);
    }

    ImGui::Render();

    renderer->EndFrame();

	float t1 = TimeServer::Instance()->GetMilliseconds();
    Log::Instance()->Write(Log::Info, "Rendering time: %f (dc: %d)", (t1 - t0), renderer->GetDrawCallsCount());
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
