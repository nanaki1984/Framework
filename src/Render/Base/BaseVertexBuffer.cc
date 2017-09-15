#include "Render/Base/BaseVertexBuffer.h"

namespace Framework {
    namespace RHI {

VertexBufferDesc::VertexBufferDesc()
{ }

VertexBufferDesc::VertexBufferDesc(const VertexBufferDesc &other)
: flags(other.flags),
  vertexDecl(other.vertexDecl),
  nVertices(other.nVertices)
{ }

VertexBufferDesc::VertexBufferDesc(VertexBufferDesc &&other)
: flags(other.flags),
  vertexDecl(std::forward<VertexDecl>(other.vertexDecl)),
  nVertices(other.nVertices)
{ }

VertexBufferDesc::~VertexBufferDesc()
{ }
        
VertexBufferDesc&
VertexBufferDesc::operator =(const VertexBufferDesc &other)
{
    flags = other.flags;
    vertexDecl = other.vertexDecl;
    nVertices = other.nVertices;
    return (*this);
}

VertexBufferDesc&
VertexBufferDesc::operator =(VertexBufferDesc &&other)
{
    flags = other.flags;
    vertexDecl = std::forward<VertexDecl>(other.vertexDecl);
    nVertices = other.nVertices;
    return (*this);
}

DefineClassInfo(Framework::RHI::BaseVertexBuffer, Framework::RHI::HardwareBuffer);

BaseVertexBuffer::BaseVertexBuffer(VertexBufferDesc &&desc)
: HardwareBuffer(desc.flags, desc.vertexDecl.GetVertexStride() * desc.nVertices)
{
    vertexDecl = std::forward<VertexDecl>(desc.vertexDecl);
}

BaseVertexBuffer::~BaseVertexBuffer()
{ }

const VertexDecl&
BaseVertexBuffer::GetVertexDecl() const
{
    return vertexDecl;
}

    } // namespace RHI
} // namespace Framework
