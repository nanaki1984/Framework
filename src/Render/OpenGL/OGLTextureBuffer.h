#pragma once

#include "Render/OpenGL/OpenGL.h"
#include "Render/Base/BaseTextureBuffer.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

class OGLTextureBuffer : public BaseTextureBuffer
{
    DeclareClassInfo;
protected:
    GLuint id;
    GLuint idStencil;
public:
    OGLTextureBuffer(const TextureBufferDesc &desc);
    virtual ~OGLTextureBuffer();

    void SetWrapMode(WrapMode _wrapMode);
    void SetFilterMode(FilterMode _filterMode);

    void Upload(const LockInfo &lockInfo, const void *data);
    void Download(const LockInfo &lockInfo, void *data);

    GLint GetId() const;
    GLint GetStencilId() const;
};

inline GLint
OGLTextureBuffer::GetId() const
{
    return id;
}

inline GLint
OGLTextureBuffer::GetStencilId() const
{
    return idStencil;
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
