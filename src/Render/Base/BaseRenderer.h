#pragma once

#include "Render/Resources/Material.h"
#include "Render/Resources/Shader.h"
#include "Render/Resources/Mesh.h"
#include "Render/Resources/ComputeShader.h"
#include "Render/RenderTarget.h"
#include "Render/MaterialParamsBlock.h"

namespace Framework {
    namespace RHI {

struct Viewport {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    float minZ;
    float maxZ;
};

class BaseRenderer : public RefCounted {
    DeclareClassInfo;
public:
    enum ClearFlags {
        ClearNone    = 0,

        ClearColor   = (1 << 0),
        ClearDepth   = (1 << 1),
        ClearStencil = (1 << 2),

        ClearAll     = ClearColor | ClearDepth | ClearStencil
    };
protected:
    RenderTarget *activeRt;

    WeakPtr<Material>      lastMaterial;
    WeakPtr<Shader>        lastShader;
    WeakPtr<Mesh>          lastMesh;
    WeakPtr<ComputeShader> lastComputeShader;

    uint32_t frameCount;
    uint32_t drawCallsCount;

    bool inBeginFrame;
public:
    BaseRenderer();
    BaseRenderer(const BaseRenderer &other) = delete;
    virtual ~BaseRenderer();

    BaseRenderer& operator =(const BaseRenderer &other) = delete;

    uint32_t GetFrameCount() const;
    uint32_t GetDrawCallsCount() const;

    bool BeginFrame();
    void ResetRenderTarget();
    void SetRenderTarget(RenderTarget *rt, CubeFace cubeFace);
    void SetViewport(const Viewport &viewport);
    void Clear(ClearFlags clearFlags, uint32_t color, float depth, uint32_t stencil);
    bool SetMaterialPass(ResourceId materialId, uint8_t pass);
    bool DrawMesh(ResourceId meshId, uint8_t subMeshIndex, const MaterialParamsBlock &params);
	bool Dispatch(ResourceId computeShaderId, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ, const MaterialParamsBlock &params);
    void EndFrame();

    void OnMeshLoaded(Mesh *mesh);
    void OnMeshUnloaded(Mesh *mesh);

    void OnShaderLoaded(Shader *shader);
    void OnShaderUnloaded(Shader *shader);

    void OnComputeShaderLoaded(ComputeShader *computeShader);
    void OnComputeShaderUnloaded(ComputeShader *computeShader);

    void OnRenderTargetCreated(RenderTarget *rt);
    void OnRenderTargetDestroyed(RenderTarget *rt);
};

inline uint32_t
BaseRenderer::GetFrameCount() const
{
    return frameCount;
}

inline uint32_t
BaseRenderer::GetDrawCallsCount() const
{
    return drawCallsCount;
}

    } // namespace RHI
} // namespace Framework

DeclareEnumStrings(Framework::RHI::BaseRenderer::ClearFlags, 8);
