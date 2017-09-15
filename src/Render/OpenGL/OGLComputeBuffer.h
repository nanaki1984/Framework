#pragma once

#include "Render/OpenGL/OpenGL.h"
#include "Render/Base/BaseComputeBuffer.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

class OGLComputeBuffer : public BaseComputeBuffer
{
    DeclareClassInfo;
protected:
    GLuint id;
	GLenum usage;
public:
	OGLComputeBuffer(const ComputeBufferDesc &desc);
    virtual ~OGLComputeBuffer();

    void Upload(const LockInfo &lockInfo, const void *data);
	void Download(const LockInfo &lockInfo, void *data);

    GLuint GetId() const;
};

inline GLuint
OGLComputeBuffer::GetId() const
{
    return id;
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
