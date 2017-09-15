#include "Render/OpenGL/OGLVertexBuffer.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

DefineClassInfo(Framework::RHI::OpenGL::OGLVertexBuffer, Framework::RHI::BaseVertexBuffer);

OGLVertexBuffer::OGLVertexBuffer(VertexBufferDesc &&desc)
: BaseVertexBuffer(std::forward<VertexBufferDesc>(desc))
{
    glGenBuffers(1, &id);

    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER,
                 size, nullptr,
                 (flags & Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    CheckOGLErrors("VertexBuffer creation");
}

OGLVertexBuffer::~OGLVertexBuffer()
{
    glDeleteBuffers(1, &id);
}
        
void
OGLVertexBuffer::Upload(const LockInfo &lockInfo, const void *data)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	
	if (lockInfo.size != 0 && flags & Dynamic) // Discard buffer if dynamic
        glBufferData(GL_ARRAY_BUFFER,
                     size, nullptr,
                     GL_DYNAMIC_DRAW);
    
    if (0 == lockInfo.size)
        glBufferData(GL_ARRAY_BUFFER,
                     size, data,
                     (this->GetFlags() & Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    else
        glBufferSubData(GL_ARRAY_BUFFER, lockInfo.offset, lockInfo.size, data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

    CheckOGLErrors("VertexBuffer upload");
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
