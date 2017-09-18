#include "Core/Application.h"
#include "Render/Base/BaseRenderer.h"
#include "Render/Resources/ResourceServer.h"

namespace Framework {
    namespace RHI {

DefineClassInfo(Framework::RHI::BaseRenderer, Framework::RefCounted);

BaseRenderer::BaseRenderer()
: activeRt(nullptr),
  frameCount(0),
  drawCallsCount(0),
  inBeginFrame(false)
{ }

BaseRenderer::~BaseRenderer()
{
    assert(!inBeginFrame);
}

bool
BaseRenderer::BeginFrame()
{
    assert(Application::IsRenderThread());
    assert(!inBeginFrame);
    inBeginFrame = true;
    drawCallsCount = 0;
    return true;
}

void
BaseRenderer::ResetRenderTarget()
{
    assert(Application::IsRenderThread());
    assert(inBeginFrame);
    if (activeRt != nullptr) {
        activeRt->Release();
        activeRt = nullptr;
    }
}

void
BaseRenderer::SetRenderTarget(RenderTarget *rt, CubeFace cubeFace)
{
    assert(Application::IsRenderThread());
    assert(inBeginFrame);
    activeRt = rt;
    activeRt->AddRef();
}

void
BaseRenderer::SetViewport(const Viewport &viewport)
{
    assert(Application::IsRenderThread());
}

void
BaseRenderer::Clear(ClearFlags clearFlags, uint32_t color, float depth, uint32_t stencil)
{
    assert(Application::IsRenderThread());
}

bool
BaseRenderer::SetMaterialPass(ResourceId materialId, uint8_t pass)
{
    assert(Application::IsRenderThread());

    if (lastMaterial.IsValid() && lastMaterial->GetId() == materialId) // ToDo: pass
        return false;

    WeakPtr<Resource> newMat = ResourceServer::Instance()->GetResourceById(materialId);
    if (newMat.IsValid())
    {
        lastMaterial = newMat.Cast<Material>();
        lastShader = lastMaterial.IsValid() ? lastMaterial->GetShader() : nullptr;

        lastComputeShader.Invalidate();

        return lastMaterial.IsValid();
    }

    return false;
}

bool
BaseRenderer::DrawMesh(ResourceId meshId, uint8_t subMeshIndex, const MaterialParamsBlock &params)
{
    assert(Application::IsRenderThread());

    ++drawCallsCount;

    if (lastMesh.IsValid() && lastMesh->GetId() == meshId)
        return false;

    WeakPtr<Resource> newMesh = ResourceServer::Instance()->GetResourceById(meshId);
    if (newMesh.IsValid())
        lastMesh = newMesh.Cast<Mesh>();

    return lastMesh.IsValid();
}

bool
BaseRenderer::Dispatch(ResourceId computeShaderId, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ, const MaterialParamsBlock &params)
{
    assert(Application::IsRenderThread());

    if (lastComputeShader.IsValid() && lastComputeShader->GetId() == computeShaderId)
        return false;

    WeakPtr<Resource> newComputeShd = ResourceServer::Instance()->GetResourceById(computeShaderId);
    if (newComputeShd.IsValid())
    {
        lastComputeShader = newComputeShd.Cast<ComputeShader>();

        lastMaterial.Invalidate();
        lastShader.Invalidate();

        return lastComputeShader.IsValid();
    }

    return false;
}

void
BaseRenderer::EndFrame()
{
    assert(Application::IsRenderThread());
    assert(inBeginFrame);
    if (activeRt != nullptr) {
        activeRt->Release();
        activeRt = nullptr;
    }

    lastMaterial.Invalidate();
    lastShader.Invalidate();
    lastMesh.Invalidate();

    inBeginFrame = false;

    ++frameCount;
}

void
BaseRenderer::OnMeshLoaded(Mesh *mesh)
{ }

void
BaseRenderer::OnMeshUnloaded(Mesh *mesh)
{ }

void
BaseRenderer::OnShaderLoaded(Shader *shader)
{ }

void
BaseRenderer::OnShaderUnloaded(Shader *shader)
{ }

void
BaseRenderer::OnComputeShaderLoaded(ComputeShader *shader)
{ }

void
BaseRenderer::OnComputeShaderUnloaded(ComputeShader *shader)
{ }

void
BaseRenderer::OnRenderTargetCreated(RenderTarget *rt)
{ }

void
BaseRenderer::OnRenderTargetDestroyed(RenderTarget *rt)
{ }

    } // namespace Base
} // namespace Render

DefineEnumStrings(Framework::RHI::BaseRenderer::ClearFlags) = {
    "ClearNone",
    "ClearTarget",
    "ClearDepth",
    "ClearTargetDepth",
    "ClearStencil",
    "ClearTargetStencil",
    "ClearDepthStencil",
    "ClearAll"
};
