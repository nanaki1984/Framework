#include "Render/OpenGL/OGLRenderer.h"
#include "Core/Collections/Hash.h"
#include "Core/Memory/MallocAllocator.h"
#include "Math/Math.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

DefineClassInfo(Framework::RHI::OpenGL::OGLRenderer, Framework::RHI::BaseRenderer);

OGLRenderer::OGLRenderer()
: fboHash(Memory::GetAllocator<MallocAllocator>()),
  vaoHash(Memory::GetAllocator<MallocAllocator>()),
  unloadedMeshes(Memory::GetAllocator<MallocAllocator>()),
  unloadedShaders(Memory::GetAllocator<MallocAllocator>()),
  rtDestroyed(Memory::GetAllocator<MallocAllocator>())
{
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVtxAttribs);
}

OGLRenderer::~OGLRenderer()
{
    for (auto it = fboHash.Begin(), end = fboHash.End(); it < end; ++it)
        glDeleteFramebuffers(1, &it->fbo);

    for (auto it = vaoHash.Begin(), end = vaoHash.End(); it < end; ++it) {
#ifdef __APPLE__
        glDeleteVertexArraysAPPLE(1, &it->vao);
#else
        glDeleteVertexArrays(1, &it->vao);
#endif
    }
}

void
OGLRenderer::SetRenderModeState(const RenderModeState *lastState, const RenderModeState &thisState)
{
    // Cull mode
    if (nullptr == lastState || lastState->cullMode != thisState.cullMode) {
        if (RenderModeState::None == thisState.cullMode)
            glDisable(GL_CULL_FACE);
        else {
            glEnable(GL_CULL_FACE);
            glCullFace(ConvertCullMode(thisState.cullMode));
        }
    }

    // Color mask
    if (nullptr == lastState || lastState->colorMask != thisState.colorMask)
        glColorMask((thisState.colorMask & RenderModeState::Red   ? GL_TRUE : GL_FALSE),
                    (thisState.colorMask & RenderModeState::Green ? GL_TRUE : GL_FALSE),
                    (thisState.colorMask & RenderModeState::Blue  ? GL_TRUE : GL_FALSE),
                    (thisState.colorMask & RenderModeState::Alpha ? GL_TRUE : GL_FALSE));

    // Depth buffer
    if (nullptr == lastState || lastState->zEnable != thisState.zEnable) {
        if(thisState.zEnable)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

    if (thisState.zEnable &&
        (nullptr == lastState || lastState->zFunc != thisState.zFunc))
        glDepthFunc(ConvertCompareFunc(thisState.zFunc));

    if (nullptr == lastState || lastState->zWriteEnable != thisState.zWriteEnable)
        glDepthMask(thisState.zWriteEnable);

    // Blending
    if (nullptr == lastState || lastState->blendEnable != thisState.blendEnable) {
        if (thisState.blendEnable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    if (thisState.blendEnable) {
        if (nullptr == lastState || lastState->blendOp != thisState.blendOp)
            glBlendEquation(ConvertBlendOp(thisState.blendOp));

        if (nullptr == lastState || lastState->blendSrcFactor != thisState.blendSrcFactor
                                 || lastState->blendDstFactor != thisState.blendDstFactor)
            glBlendFunc(ConvertBlendFactor(thisState.blendSrcFactor),
                        ConvertBlendFactor(thisState.blendDstFactor));
    }

    // ToDo: Alpha test
    // ToDo: Stencil

    if (nullptr == lastState ||
        lastState->scissorTestEnabled != thisState.scissorTestEnabled ||
        (thisState.scissorTestEnabled && lastState->scissorTestRect != thisState.scissorTestRect))
    {
        if (thisState.scissorTestEnabled)
        {
            if (!lastState->scissorTestEnabled)
                glEnable(GL_SCISSOR_TEST);

            glScissor(thisState.scissorTestRect[0], thisState.scissorTestRect[1], thisState.scissorTestRect[2], thisState.scissorTestRect[3]);
        }
        else
            glDisable(GL_SCISSOR_TEST);
    }

    // ToDo: clip planes

    lastState = &thisState;

    CheckOGLErrors("SetRenderMode");
}

void
OGLRenderer::CreateFBO(RenderTarget *rt, CubeFace cubeFace)
{
    FBO fbo;
    fbo.rt = rt;
    fbo.cubeFace = cubeFace;

    glGenFramebuffers(1, &fbo.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo);

    SmartPtr<OGLTextureBuffer> colorBuffer = SmartPtr<OGLTextureBuffer>::CastFrom(rt->GetColor()->GetBuffer());
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           (BaseTextureBuffer::Texture2D == colorBuffer->GetType() ? GL_TEXTURE_2D : ConvertCubeFace(cubeFace)),
                           colorBuffer->GetId(),
                           0);

    if (rt->GetDepth().IsValid()) {
        SmartPtr<OGLTextureBuffer> depthBuffer = SmartPtr<OGLTextureBuffer>::CastFrom(rt->GetDepth()->GetBuffer());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depthBuffer->GetId());

        GLuint stencilID = depthBuffer->GetStencilId();
        if (stencilID) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER,
                                      stencilID);
        } else {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER,
                                      0);
        }
    } else {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  0);
    }
#ifdef _DEBUG
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert2(GL_FRAMEBUFFER_COMPLETE == fboStatus, "FBO status error!");
#endif

    fboHash.Add(rt->GetId(), fbo);
}

void
OGLRenderer::CreateVAO(const WeakPtr<Shader> &shader, Mesh *mesh)
{
    VAO vao;
    vao.shaderId = shader->GetId();
    vao.mesh = mesh;
#ifdef __APPLE__
    glGenVertexArraysAPPLE(1, &vao.vao);
    glBindVertexArrayAPPLE(vao.vao);
#else
    glGenVertexArrays(1, &vao.vao);
    glBindVertexArray(vao.vao);
#endif

    OGLShaderProgram *shaderProgram = shader->GetRenderData().program.Get();

    auto &meshRenderData = mesh->GetRenderData();
    const VertexDecl &vertexDecl = meshRenderData.vd;
    OGLVertexBuffer *vb = meshRenderData.vb.Get();
    OGLIndexBuffer *ib = meshRenderData.ib.Get();

    glBindBuffer(GL_ARRAY_BUFFER, vb->GetId());

    uint32_t i = 0;
    for (; i < (uint32_t)maxVtxAttribs; ++i)
        glDisableVertexAttribArray(i);

    uint32_t nElem   = vertexDecl.GetElementsCount(),
             vtxSize = vertexDecl.GetVertexStride(),
             nTex    = 0;

    BaseShaderProgram::ShaderParam shdParam;
    for (i = 0; i < nElem; ++i) {
        const VertexDecl::VertexElement& elem = vertexDecl.GetElement(i);

        switch (elem.type) {
            case VertexDecl::XYZ:
                if (shaderProgram->TryGetParam("position", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size >> 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
                break;
            case VertexDecl::Normal:
                if (shaderProgram->TryGetParam("normal", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size >> 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
                break;
            case VertexDecl::Tangent:
                if (shaderProgram->TryGetParam("tangent", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size >> 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
                break;
            case VertexDecl::Binormal:
                if (shaderProgram->TryGetParam("binormal", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size >> 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
                break;
            case VertexDecl::BlendWeights:
                if (shaderProgram->TryGetParam("blendWeights", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size >> 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
                break;
            case VertexDecl::BlendIndices:
                if (shaderProgram->TryGetParam("blendIndices", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size >> 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
                break;
            case VertexDecl::TexCoord:
                if (0 == nTex && (shaderProgram->TryGetParam("uv", shdParam) || shaderProgram->TryGetParam("uv0", shdParam))) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size >> 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                } else if (nTex > 0) {
                    String uvName("uv");
                    uvName += ('0' + nTex);
                    if (shaderProgram->TryGetParam(uvName, shdParam)) {
                        glEnableVertexAttribArray(shdParam.regOrIndex);
                        glVertexAttribPointer(shdParam.regOrIndex,
                                              elem.size >> 2,
                                              GL_FLOAT,
                                              GL_FALSE,
                                              vtxSize,
                                              (const void*)elem.offset);
                    }
                }
                ++nTex;
                break;
            case VertexDecl::Color0:
                if (shaderProgram->TryGetParam("color", shdParam) || shaderProgram->TryGetParam("color0", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size,
                                          GL_UNSIGNED_BYTE,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
            case VertexDecl::Color1:
                if (shaderProgram->TryGetParam("color1", shdParam)) {
                    glEnableVertexAttribArray(shdParam.regOrIndex);
                    glVertexAttribPointer(shdParam.regOrIndex,
                                          elem.size,
                                          GL_UNSIGNED_BYTE,
                                          GL_FALSE,
                                          vtxSize,
                                          (const void*)elem.offset);
                }
                break;
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->GetId());

    CheckOGLErrors("VertexArrayObject");

    vaoHash.Add(mesh->GetId(), vao);
}

void
OGLRenderer::FreeUnusedObjects()
{
    for (auto it = unloadedMeshes.Begin(), end = unloadedMeshes.End(); it != end; ++it)
    {
        VAO *vao = vaoHash.Get(*it);
        while (vao != nullptr) {
            VAO *tmp = vao;
#ifdef __APPLE__
            glDeleteVertexArraysAPPLE(1, &vao->vao);
#else
            glDeleteVertexArrays(1, &vao->vao);
#endif
            vao = vaoHash.Next(vao);
            vaoHash.Remove(tmp);
        }
    }
    unloadedMeshes.Clear();

    for (auto it = unloadedShaders.Begin(), end = unloadedShaders.End(); it != end; ++it)
    {
        VAO *hashIt = vaoHash.Begin();
        while (hashIt < vaoHash.End()) {
            if (hashIt->shaderId == (*it)) {
#ifdef __APPLE__
                glDeleteVertexArraysAPPLE(1, &hashIt->vao);
#else
                glDeleteVertexArrays(1, &hashIt->vao);
#endif
                vaoHash.Remove(hashIt);
            }
            else ++hashIt;
        }
    }
    unloadedShaders.Clear();

    for (auto it = rtDestroyed.Begin(), end = rtDestroyed.End(); it != end; ++it)
    {
        FBO *fbo = fboHash.Get(*it);
        while (fbo != nullptr) {
            FBO *tmp = fbo;
            glDeleteFramebuffers(1, &tmp->fbo);
            fbo = fboHash.Next(fbo);
            fboHash.Remove(tmp);
        }
    }
    rtDestroyed.Clear();
}

void
OGLRenderer::ApplyFloatParams(OGLShaderProgram *program, const Materials::FloatParam *begin, const Materials::FloatParam *end)
{
    BaseShaderProgram::ShaderParam shdParam;
    for (auto it = begin; it < end; ++it) {
        if (program->TryGetParam(it->name, shdParam) && GL_FLOAT == shdParam.type)
            glUniform1fv(shdParam.regOrIndex, 1, &it->value);
    }
}

void
OGLRenderer::ApplyVectorParams(OGLShaderProgram *program, const Materials::VectorParam *begin, const Materials::VectorParam *end)
{
    BaseShaderProgram::ShaderParam shdParam;
    for (auto it = begin; it < end; ++it) {
        if (program->TryGetParam(it->name, shdParam)) {
            switch (shdParam.type) {
                case GL_FLOAT_VEC2:
                    glUniform1fv(shdParam.regOrIndex, 2, it->value.xyzw);
                    break;
                case GL_FLOAT_VEC3:
                    glUniform1fv(shdParam.regOrIndex, 3, it->value.xyzw);
                    break;
                case GL_FLOAT_VEC4:
                    glUniform1fv(shdParam.regOrIndex, 4, it->value.xyzw);
                    break;
                default:
                    break;
            }
        }
    }
}

void
OGLRenderer::ApplyMatrixParams(OGLShaderProgram *program, const Materials::MatrixParam *begin, const Materials::MatrixParam *end)
{
    BaseShaderProgram::ShaderParam shdParam;
    for (auto it = begin; it < end; ++it) {
        if (program->TryGetParam(it->name, shdParam) && GL_FLOAT_MAT4 == shdParam.type)
            glUniformMatrix4fv(shdParam.regOrIndex, 1, GL_FALSE, static_cast<const float*>(it->value));
    }
}

void
OGLRenderer::ApplyTextureParams(OGLShaderProgram *program, const Materials::TextureParam *begin, const Materials::TextureParam *end)
{
    BaseShaderProgram::ShaderParam shdParam;
    for (auto it = begin; it < end; ++it) {
        if (program->TryGetParam(it->name, shdParam)) {
            const OGLTextureBuffer *tex = static_cast<const OGLTextureBuffer*>(it->value->GetBuffer().Get());
            switch (shdParam.type) {
                case GL_SAMPLER_1D:
                    if (BaseTextureBuffer::Texture1D == tex->GetType()) {
                        glActiveTexture(GL_TEXTURE0 + shdParam.regOrIndex);
                        glBindTexture(GL_TEXTURE_1D, tex->GetId());
                    }
                    break;
                case GL_SAMPLER_2D:
                    if (BaseTextureBuffer::Texture2D == tex->GetType()) {
                        glActiveTexture(GL_TEXTURE0 + shdParam.regOrIndex);
                        glBindTexture(GL_TEXTURE_2D, tex->GetId());
                    }
                    break;
                case GL_SAMPLER_CUBE:
                    if (BaseTextureBuffer::TextureCube == tex->GetType()) {
                        glActiveTexture(GL_TEXTURE0 + shdParam.regOrIndex);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, tex->GetId());
                    }
                    break;
                case GL_SAMPLER_3D:/*
                    if (BaseTextureBuffer::Texture3D == tex->GetType()) {
                        glActiveTexture(GL_TEXTURE0 + shdParam.regOrIndex);
                        glBindTexture(GL_TEXTURE_3D, tex->GetId());
                    }*/
                    break;
                default:
                    break;
            }
        }
    }
}

void
OGLRenderer::ApplyBufferParams(OGLShaderProgram *program, const Materials::BufferParam *begin, const Materials::BufferParam *end)
{
    BaseShaderProgram::ShaderParam shdParam;
    for (auto it = begin; it < end; ++it) {
        if (program->TryGetParam(it->name, shdParam) && GL_SHADER_STORAGE_BUFFER == shdParam.type)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, shdParam.regOrIndex, it->value->GetId());
    }
}

bool
OGLRenderer::BeginFrame()
{
    return BaseRenderer::BeginFrame();
}

void
OGLRenderer::ResetRenderTarget()
{
    BaseRenderer::ResetRenderTarget();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CheckOGLErrors("ResetRenderTarget");
}

void
OGLRenderer::SetRenderTarget(RenderTarget *rt, CubeFace cubeFace)
{
    BaseRenderer::SetRenderTarget(rt, cubeFace);

    FBO *fbo = fboHash.Get(rt->GetId());
    while (fbo != nullptr) {
        if (fbo->rt == rt && fbo->cubeFace == cubeFace)
            break;
        fbo = fboHash.Next(fbo);
    }

    if (nullptr == fbo) {
        this->CreateFBO(rt, cubeFace);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    }

    CheckOGLErrors("SetRenderTarget");
}

void
OGLRenderer::SetViewport(const Viewport &viewport)
{
    BaseRenderer::SetViewport(viewport);

    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glDepthRange(viewport.minZ, viewport.maxZ);

    CheckOGLErrors("SetViewport");
}

void
OGLRenderer::Clear(ClearFlags clearFlags, uint32_t color, float depth, uint32_t stencil)
{
    BaseRenderer::Clear(clearFlags, color, depth, stencil);

    if (ClearNone != clearFlags) {
        GLbitfield oglClearFlags = 0;

        if (clearFlags & ClearColor) {
            oglClearFlags |= GL_COLOR_BUFFER_BIT;
            glClearColor(static_cast<float>((color&0x00ff0000)>>16) / 255.0f,
                         static_cast<float>((color&0x0000ff00)>> 8) / 255.0f,
                         static_cast<float>((color&0x000000ff)>> 0) / 255.0f,
                         static_cast<float>((color&0xff000000)>>24) / 255.0f);
        }

        if (clearFlags & ClearDepth) {
            oglClearFlags |= GL_DEPTH_BUFFER_BIT;
            glClearDepth(depth);
        }

        if (clearFlags & ClearStencil) {
            oglClearFlags |= GL_STENCIL_BUFFER_BIT;
            glClearStencil(stencil);
        }

        // save old color & depth masks
        GLint oldColorMasks[4], oldDepthMask;
        glGetIntegerv(GL_COLOR_WRITEMASK, oldColorMasks);
        glGetIntegerv(GL_DEPTH_WRITEMASK, &oldDepthMask);

        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(oglClearFlags);

        // reset old color & depth masks
        glColorMask(oldColorMasks[0], oldColorMasks[1], oldColorMasks[2], oldColorMasks[3]);
        glDepthMask(oldDepthMask);

        CheckOGLErrors("Clear");
    }
}

bool
OGLRenderer::SetMaterialPass(ResourceId materialId, uint8_t pass)
{
    const Material *prevMaterial = lastMaterial;
    if (BaseRenderer::SetMaterialPass(materialId, pass) && lastMaterial.IsValid()) {
        auto &matRenderData = lastMaterial->GetRenderData();
        this->SetRenderModeState(nullptr == prevMaterial ? nullptr : &prevMaterial->GetRenderData().renderMode, matRenderData.renderMode);

        auto &shdRenderData = lastShader->GetRenderData();
        assert(shdRenderData.program->IsLinked());
        OGLShaderProgram *oglProg = shdRenderData.program.Get();
        glUseProgram(oglProg->GetProgramHandle());
        CheckOGLErrors("UseShaderProgram");

        this->ApplyFloatParams(oglProg, matRenderData.parameters.FloatParamsBegin(), matRenderData.parameters.FloatParamsEnd());
        this->ApplyVectorParams(oglProg, matRenderData.parameters.VectorParamsBegin(), matRenderData.parameters.VectorParamsEnd());
        this->ApplyMatrixParams(oglProg, matRenderData.parameters.MatrixParamsBegin(), matRenderData.parameters.MatrixParamsEnd());
        CheckOGLErrors("MaterialUniforms");

        this->ApplyTextureParams(oglProg, matRenderData.parameters.TextureParamsBegin(), matRenderData.parameters.TextureParamsEnd());
		CheckOGLErrors("BindTextures");
    }

    return lastMaterial.IsValid();
}

bool
OGLRenderer::DrawMesh(ResourceId meshId, uint8_t subMeshIndex, const MaterialParamsBlock &params)
{
    if (BaseRenderer::DrawMesh(meshId, subMeshIndex, params)) {
        VAO *vao = vaoHash.Get(meshId);
        while (vao != nullptr) {
            if (vao->shaderId == lastShader->GetId())
                break;
            vao = vaoHash.Next(vao);
        }

        if (nullptr == vao)
            this->CreateVAO(lastShader, lastMesh);
        else {
#ifdef __APPLE__
            glBindVertexArrayAPPLE(vao->vao);
#else
            glBindVertexArray(vao->vao);
#endif
        }

        CheckOGLErrors("MeshBuffers");
    }

    if (!lastMesh.IsValid())
        return false;

    OGLShaderProgram *oglProg = lastShader->GetRenderData().program.Get();

    this->ApplyFloatParams(oglProg, params.FloatParamsBegin(), params.FloatParamsEnd());
    this->ApplyVectorParams(oglProg, params.VectorParamsBegin(), params.VectorParamsEnd());
    this->ApplyMatrixParams(oglProg, params.MatrixParamsBegin(), params.MatrixParamsEnd());
    CheckOGLErrors("MeshUniforms");

    this->ApplyTextureParams(oglProg, params.TextureParamsBegin(), params.TextureParamsEnd());
    CheckOGLErrors("MeshTextures");

    this->ApplyBufferParams(oglProg, params.BufferParamsBegin(), params.BufferParamsEnd());
    CheckOGLErrors("MeshBuffers");

    auto &meshRenderData = lastMesh->GetRenderData();
    IndexSize idxSize = meshRenderData.ib->GetIndexSize();
    const DrawPrimitives &drawCall = meshRenderData.dc[subMeshIndex];

    uint32_t idxCount = 0;
    switch (drawCall.primType) {
        case DrawPrimitives::PointList:
            idxCount = drawCall.nPrimitives;
            break;
        case DrawPrimitives::LineList:
            idxCount = drawCall.nPrimitives / 2;
            break;
        case DrawPrimitives::LineStrip:
            idxCount = drawCall.nPrimitives + 1;
            break;
        case DrawPrimitives::TriangleList:
            idxCount = drawCall.nPrimitives * 3;
            break;
        case DrawPrimitives::TriangleStrip:
            idxCount = drawCall.nPrimitives + 2;
            break;
        case DrawPrimitives::TriangleFan:
            idxCount = drawCall.nPrimitives + 2;
            break;
        default:
            assert(false);
            break;
    }

    glDrawElements(ConvertPrimitiveType(drawCall.primType),
                   idxCount,
                   (IndexWord == idxSize ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT),
                   ((char*)nullptr) + (idxSize * drawCall.startIndex));
    CheckOGLErrors("DrawPrimitives");

    return true;
}

bool
OGLRenderer::Dispatch(ResourceId computeShaderId, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ, const MaterialParamsBlock &params)
{
    if (BaseRenderer::Dispatch(computeShaderId, numGroupsX, numGroupsY, numGroupsZ, params))
    {
        auto &cmpShdRenderData = lastComputeShader->GetRenderData();
        assert(cmpShdRenderData.program->IsLinked());
        OGLShaderProgram *oglProg = cmpShdRenderData.program.Get();
        glUseProgram(oglProg->GetProgramHandle());
        CheckOGLErrors("UseShaderProgram");
    }

    if (!lastComputeShader.IsValid())
        return false;

    OGLShaderProgram *oglProg = lastComputeShader->GetRenderData().program.Get();

    this->ApplyFloatParams(oglProg, params.FloatParamsBegin(), params.FloatParamsEnd());
    this->ApplyVectorParams(oglProg, params.VectorParamsBegin(), params.VectorParamsEnd());
    this->ApplyMatrixParams(oglProg, params.MatrixParamsBegin(), params.MatrixParamsEnd());
    CheckOGLErrors("DispatchUniforms");

    this->ApplyTextureParams(oglProg, params.TextureParamsBegin(), params.TextureParamsEnd());
    CheckOGLErrors("DispatchTextures");

    this->ApplyBufferParams(oglProg, params.BufferParamsBegin(), params.BufferParamsEnd());
    CheckOGLErrors("DispatchBuffers");

    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
    CheckOGLErrors("DispatchCompute");

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // ToDo: for now we're interested only in buffers (images not supperted yet)
    CheckOGLErrors("DispatchMemoryBarrier");

    return true;
}

void
OGLRenderer::EndFrame()
{
    glFlush();

    BaseRenderer::EndFrame();
}

void
OGLRenderer::OnEndFrameCommands()
{
    this->FreeUnusedObjects();
}

void
OGLRenderer::OnMeshUnloaded(Mesh *mesh)
{
    unloadedMeshes.PushBack(mesh->GetId());
}

void
OGLRenderer::OnShaderUnloaded(Shader *shader)
{
    unloadedShaders.PushBack(shader->GetId());
}

void
OGLRenderer::OnRenderTargetDestroyed(RenderTarget *rt)
{
    rtDestroyed.PushBack(rt->GetId());
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
