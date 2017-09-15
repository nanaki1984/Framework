#pragma once

#include "Render/Base/HardwareBuffer.h"
#include "Render/VertexDecl.h"

namespace Framework {
    namespace RHI {

class BaseShaderProgram;

class VertexBufferDesc {
public:
    HardwareBuffer::Flags flags;
    VertexDecl            vertexDecl;
    uint32_t              nVertices;

    VertexBufferDesc();
    VertexBufferDesc(const VertexBufferDesc &other);
    VertexBufferDesc(VertexBufferDesc &&other);
    ~VertexBufferDesc();

    VertexBufferDesc& operator =(const VertexBufferDesc &other);
    VertexBufferDesc& operator =(VertexBufferDesc &&other);
};

class BaseVertexBuffer : public HardwareBuffer {
    DeclareClassInfo;
protected:
    VertexDecl vertexDecl;
public:
    BaseVertexBuffer(VertexBufferDesc &&desc);
    virtual ~BaseVertexBuffer();

    const VertexDecl& GetVertexDecl() const;
};

    } // namespace RHI
} // namespace Framework
