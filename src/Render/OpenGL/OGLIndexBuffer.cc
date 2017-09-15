#include "Render/OpenGL/OGLIndexBuffer.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

DefineClassInfo(Framework::RHI::OpenGL::OGLIndexBuffer, Framework::RHI::BaseIndexBuffer);

OGLIndexBuffer::OGLIndexBuffer(const IndexBufferDesc &desc)
: BaseIndexBuffer(desc)
{
    glGenBuffers(1, &id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 size, nullptr,
                 (flags & Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CheckOGLErrors("IndexBuffer creation");
}

OGLIndexBuffer::~OGLIndexBuffer ( )
{
    glDeleteBuffers(1, &id);
}

void
OGLIndexBuffer::Upload(const LockInfo &lockInfo, const void *data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

	if (lockInfo.size != 0 && flags & Dynamic) // Discard buffer if dynamic
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     size, nullptr,
                     GL_DYNAMIC_DRAW);

    if (0 == lockInfo.size)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     size, data,
                     (this->GetFlags() & Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    else
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, lockInfo.offset, lockInfo.size, data);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CheckOGLErrors("IndexBuffer upload");
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
