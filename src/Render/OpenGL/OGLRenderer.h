#pragma once

#include "Render/OpenGL/OpenGL.h"
#include "Render/Base/BaseRenderer.h"
#include "Core/Collections/Hash_type.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

class OGLRenderer : public BaseRenderer {
    DeclareClassInfo;
protected:
    struct FBO {
        RenderTarget *rt;
        CubeFace cubeFace;
        GLuint fbo;
    };

    struct VAO {
        ResourceId shaderId;
        Mesh *mesh;
        GLuint vao;
    };

    Hash<FBO> fboHash;
    Hash<VAO> vaoHash;

    GLint maxVtxAttribs;

    Array<ResourceId> unloadedMeshes;
    Array<ResourceId> unloadedShaders;
    Array<uint32_t>   rtDestroyed;

    void SetRenderModeState(const RenderModeState *lastState, const RenderModeState &thisState);
    void CreateFBO(RenderTarget *rt, CubeFace cubeFace);
    void CreateVAO(const WeakPtr<Shader> &shader, Mesh *mesh);
    void FreeUnusedObjects();

    void ApplyFloatParams(OGLShaderProgram *program, const Materials::FloatParam *begin, const Materials::FloatParam *end);
    void ApplyVectorParams(OGLShaderProgram *program, const Materials::VectorParam *begin, const Materials::VectorParam *end);
    void ApplyMatrixParams(OGLShaderProgram *program, const Materials::MatrixParam *begin, const Materials::MatrixParam *end);
    void ApplyTextureParams(OGLShaderProgram *program, const Materials::TextureParam *begin, const Materials::TextureParam *end);
    void ApplyBufferParams(OGLShaderProgram *program, const Materials::BufferParam *begin, const Materials::BufferParam *end);
public:
    OGLRenderer();
    virtual ~OGLRenderer();

    bool BeginFrame();
    void ResetRenderTarget();
    void SetRenderTarget(RenderTarget *rt, CubeFace cubeFace);
    void SetViewport(const Viewport &viewport);
    void Clear(ClearFlags clearFlags, uint32_t color, float depth, uint32_t stencil);
    bool SetMaterialPass(ResourceId materialId, uint8_t pass);
    bool DrawMesh(ResourceId meshId, uint8_t subMeshIndex, const MaterialParamsBlock &params);
    bool Dispatch(ResourceId computeShaderId, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ, const MaterialParamsBlock &params);
    void EndFrame();

    void OnEndFrameCommands();
    void OnMeshUnloaded(Mesh *mesh);
    void OnShaderUnloaded(Shader *shader);
    void OnRenderTargetDestroyed(RenderTarget *rt);
};

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
