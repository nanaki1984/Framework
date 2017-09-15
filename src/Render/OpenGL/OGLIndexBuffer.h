#pragma once

#include "Render/OpenGL/OpenGL.h"
#include "Render/Base/BaseIndexBuffer.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

class OGLIndexBuffer : public BaseIndexBuffer
{
    DeclareClassInfo;
protected:
    GLuint id;
public:
    OGLIndexBuffer(const IndexBufferDesc &desc);
    virtual ~OGLIndexBuffer();

    void Upload(const LockInfo &lockInfo, const void *data);

    GLuint GetId() const;
};

inline GLuint
OGLIndexBuffer::GetId() const
{
    return id;
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
