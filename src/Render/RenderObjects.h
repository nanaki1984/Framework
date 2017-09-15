#pragma once

#if 1 // OpenGL renderer

#include "Render/OpenGL/OGLVertexBuffer.h"
#include "Render/OpenGL/OGLIndexBuffer.h"
#include "Render/OpenGL/OGLTextureBuffer.h"
#include "Render/OpenGL/OGLShaderProgram.h"
#include "Render/OpenGL/OGLComputeBuffer.h"

namespace Framework {
	namespace RHI {

class VertexBuffer : public OpenGL::OGLVertexBuffer {
public:
    VertexBuffer(VertexBufferDesc &&desc) : OGLVertexBuffer(std::forward<VertexBufferDesc>(desc)) { }
    virtual ~VertexBuffer() { }
};

class IndexBuffer : public OpenGL::OGLIndexBuffer {
public:
    IndexBuffer(const IndexBufferDesc &desc) : OGLIndexBuffer(desc) { }
    virtual ~IndexBuffer() { }
};

class TextureBuffer : public OpenGL::OGLTextureBuffer {
public:
    TextureBuffer(const TextureBufferDesc &desc) : OGLTextureBuffer(desc) { }
    virtual ~TextureBuffer() { }
};

class ShaderProgram : public OpenGL::OGLShaderProgram { };

class ComputeBuffer : public OpenGL::OGLComputeBuffer {
public:
    ComputeBuffer(const ComputeBufferDesc &desc) : OGLComputeBuffer(desc) { }
    virtual ~ComputeBuffer() { }
};

    } // namespace RHI
} // namespace Framework

#endif
