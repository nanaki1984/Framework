#include "Render/OpenGL/OGLComputeBuffer.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

DefineClassInfo(Framework::RHI::OpenGL::OGLComputeBuffer, Framework::RHI::BaseComputeBuffer);

OGLComputeBuffer::OGLComputeBuffer(const ComputeBufferDesc &desc)
: BaseComputeBuffer(desc)
{
    glGenBuffers(1, &id);

	switch (type)
	{
		case CBType_Default:
			usage = flags & Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
			break;
		case CBType_GPUOnly:
			usage = flags & Dynamic ? GL_DYNAMIC_COPY : GL_STATIC_COPY;
			break;
	}

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, stride * count, nullptr, usage);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    CheckOGLErrors("ComputeBuffer creation");
}

OGLComputeBuffer::~OGLComputeBuffer( )
{
    glDeleteBuffers(1, &id);
}

void
OGLComputeBuffer::Upload(const LockInfo &lockInfo, const void *data)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	GLvoid *p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(static_cast<uint8_t*>(p) + lockInfo.offset, data, (0 == lockInfo.size ? size - lockInfo.offset : lockInfo.size));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    CheckOGLErrors("ComputeBuffer upload");
}

void
OGLComputeBuffer::Download(const LockInfo &lockInfo, void *data)
{
	BaseComputeBuffer::Download(lockInfo, data);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	GLvoid *p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	memcpy(data, static_cast<uint8_t*>(p) + lockInfo.offset, (0 == lockInfo.size ? size - lockInfo.offset : lockInfo.size));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	CheckOGLErrors("ComputeBuffer download");
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
