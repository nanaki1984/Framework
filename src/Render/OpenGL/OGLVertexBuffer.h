#pragma once

#include "Render/OpenGL/OpenGL.h"
#include "Render/Base/BaseVertexBuffer.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

class OGLVertexBuffer : public BaseVertexBuffer
{
    DeclareClassInfo;
protected:
    GLuint id;
public:
    OGLVertexBuffer(VertexBufferDesc &&desc);
    virtual ~OGLVertexBuffer();

    void Upload(const LockInfo &lockInfo, const void *data);

    GLuint GetId() const;
};

inline GLuint
OGLVertexBuffer::GetId() const
{
    return id;
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
