#include "Render/OpenGL/OGLTextureBuffer.h"
#include "Core/Memory/ScratchAllocator.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

DefineClassInfo(Framework::RHI::OpenGL::OGLTextureBuffer, Framework::RHI::BaseTextureBuffer);

OGLTextureBuffer::OGLTextureBuffer(const TextureBufferDesc &desc)
: BaseTextureBuffer(desc)
{
    int cubeFace;

    GLint oglFormat,
          oglSrcType,
          oglSrcFormat;

    ConvertImageFormat(format, oglFormat, oglSrcType, oglSrcFormat);

    if (flags & DepthStencil) {
        assert(Texture2D == type);

        uint32_t depthBits, stencilBits;
        switch (format) {
            case ImageFormat::D16:
                depthBits   = GL_DEPTH_COMPONENT16;
                stencilBits = 0;
                break;
            case ImageFormat::D15S1:
                depthBits   = GL_DEPTH_COMPONENT16;  // in opengl there is no 15bit depth
                stencilBits = GL_STENCIL_INDEX1;
                break;
            case ImageFormat::D32:
                depthBits   = GL_DEPTH_COMPONENT32;
                stencilBits = 0;
                break;
            case ImageFormat::D24X8:
                depthBits   = GL_DEPTH_COMPONENT24;
                stencilBits = 0;
                break;
            case ImageFormat::D24S8:
                depthBits   = GL_DEPTH_COMPONENT24;
                stencilBits = GL_STENCIL_INDEX8;
                break;
            default:
                assert(false);
                depthBits = 0;
                stencilBits = 0;
                break;
        };

        glGenRenderbuffers(1, &id);

        glBindRenderbuffer(GL_RENDERBUFFER, id);
        glRenderbufferStorage(GL_RENDERBUFFER, depthBits, width, height);

        if (stencilBits) {
			glGenRenderbuffers(1, &idStencil);
			glBindRenderbuffer(GL_RENDERBUFFER, idStencil);
			glRenderbufferStorage(GL_RENDERBUFFER, stencilBits, width, height);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        CheckOGLErrors("DepthStencil creation");

        return;
    }

    if (flags & RenderTarget) {
        if (GL_HALF_FLOAT == oglSrcType) // ATI hack, not quite sure
            oglSrcType = GL_FLOAT;

        assert(Texture1D != type && Texture3D != type);

        glGenTextures(1, &id);

        if (Texture2D == type) {
            glBindTexture(GL_TEXTURE_2D, id);

            glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglSrcFormat, oglSrcType, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, mipmapsRangeMin);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapsRangeMax);

            this->SetWrapMode(wrapMode);
            this->SetFilterMode(filterMode);
        }

        if (TextureCube == type) {
            glBindTexture(GL_TEXTURE_CUBE_MAP, id);

            for (cubeFace = 0; cubeFace < 6; ++cubeFace)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeFace,
                             0, oglFormat, width, height, 0, oglSrcFormat, oglSrcType, nullptr);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, mipmapsRangeMin);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmapsRangeMax);

            this->SetWrapMode(wrapMode);
            this->SetFilterMode(filterMode);
        }

        CheckOGLErrors("RenderTarget creation");

        return;
    }

    if (GL_HALF_FLOAT == oglSrcType) // ATI hack, not quite sure
        oglSrcType = GL_FLOAT;

    GLenum oglTexType = ConvertTextureType(type);

    glGenTextures(1, &id);
    glBindTexture(oglTexType, id);

    ImageFormat texFrmt = ImageFormat(format);
    bool compressed = ImageFormat::Compressed == (texFrmt.GetFlags() & ImageFormat::Compressed);

    uint32_t sz = 0;
    void *data = nullptr;

    switch (type) {
        case Texture1D:
            if (compressed) {
                sz = texFrmt.GetSurfaceSize(width, 1);
                data = Memory::GetAllocator<ScratchAllocator>().Allocate(sz, 1);
                glCompressedTexImage1D(oglTexType, 0, oglFormat, width, 0, sz, data);
                Memory::GetAllocator<ScratchAllocator>().Free(data);
            } else
                glTexImage1D(oglTexType, 0, oglFormat, width, 0, oglSrcFormat, oglSrcType, nullptr);
            break;
        case Texture2D:
            if (compressed) {
                sz = texFrmt.GetSurfaceSize(width, height);
                data = Memory::GetAllocator<ScratchAllocator>().Allocate(sz, 1);
                glCompressedTexImage2D(oglTexType, 0, oglFormat, width, height, 0, sz, data);
                Memory::GetAllocator<ScratchAllocator>().Free(data);
            } else
                glTexImage2D(oglTexType, 0, oglFormat, width, height, 0, oglSrcFormat, oglSrcType, nullptr);
            break;
        case TextureCube:
            if (compressed) {
                sz = texFrmt.GetSurfaceSize(width, height);
                data = Memory::GetAllocator<ScratchAllocator>().Allocate(sz, 1);
            }
            for (cubeFace = 0; cubeFace < 6; ++cubeFace) {
                if (compressed)
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeFace,
                                           0, oglFormat, width, height, 0, sz, data);
                else
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeFace,
                                 0, oglFormat, width, height, 0, oglSrcFormat, oglSrcType, nullptr);
            }
            if (compressed)
                Memory::GetAllocator<ScratchAllocator>().Free(data);
            break;
        case Texture3D:
            assert(false);
            break;
    }

	CheckOGLErrors("Texture creation");
	
	glTexParameteri(oglTexType, GL_TEXTURE_BASE_LEVEL, mipmapsRangeMin);
    glTexParameteri(oglTexType, GL_TEXTURE_MAX_LEVEL, mipmapsRangeMax);

	CheckOGLErrors("Texture parameters");
	
	this->SetWrapMode(wrapMode);
    this->SetFilterMode(filterMode);
}

OGLTextureBuffer::~OGLTextureBuffer()
{
    if (flags & DepthStencil) {
        glDeleteRenderbuffers(1, &id);

        if (idStencil)
            glDeleteRenderbuffers(1, &idStencil);
    } else
        glDeleteTextures(1, &id);
}

void
OGLTextureBuffer::SetWrapMode(WrapMode _wrapMode)
{
    BaseTextureBuffer::SetWrapMode(_wrapMode);

	GLenum oglTexType = ConvertTextureType(type);
    switch (wrapMode) {
        case BaseTextureBuffer::Clamp:
            glTexParameteri(oglTexType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(oglTexType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case BaseTextureBuffer::Repeat:
            glTexParameteri(oglTexType, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(oglTexType, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
    }

	CheckOGLErrors("Texture parameters");
}

void
OGLTextureBuffer::SetFilterMode(FilterMode _filterMode)
{
    BaseTextureBuffer::SetFilterMode(_filterMode);

	GLenum oglTexType = ConvertTextureType(type);
    switch (filterMode) {
        case BaseTextureBuffer::Nearest:
            glTexParameteri(oglTexType, GL_TEXTURE_MIN_FILTER, this->UsesMipmaps() ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
            glTexParameteri(oglTexType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case BaseTextureBuffer::Bilinear:
            glTexParameteri(oglTexType, GL_TEXTURE_MIN_FILTER, this->UsesMipmaps() ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
            glTexParameteri(oglTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case BaseTextureBuffer::Trilinear:
            glTexParameteri(oglTexType, GL_TEXTURE_MIN_FILTER, this->UsesMipmaps() ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glTexParameteri(oglTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
    }

	CheckOGLErrors("Texture parameters");
}

void
OGLTextureBuffer::Upload(const LockInfo &lockInfo, const void *data)
{
    assert(0 == (flags & RenderTarget) && 0 == (flags & DepthStencil));

    GLenum oglTexType = ConvertTextureType(type);
    glBindTexture(oglTexType, id);

    GLint oglFormat,
          oglSrcType,
          oglSrcFormat,
          oglLevel = lockInfo.mipmapLevel;

    bool generateMipmaps = false;
    if (LockInfo::kGenerateMipmaps == lockInfo.mipmapLevel) {
        oglLevel = 0;
        generateMipmaps = true;
    }

    ConvertImageFormat(format, oglFormat, oglSrcType, oglSrcFormat);

    ImageFormat texFrmt = ImageFormat(format);
    bool compressed = ImageFormat::Compressed == (texFrmt.GetFlags() & ImageFormat::Compressed);

    uint32_t w = MipmapSize(width, oglLevel),
             h = MipmapSize(height, oglLevel);

    switch (type) {
        case Texture1D:
            if (compressed) {
                if (lockInfo.xOffset == 0 && lockInfo.yOffset == 0 && lockInfo.width == 0 && lockInfo.height == 0)
                    glCompressedTexImage1D(GL_TEXTURE_1D, oglLevel, oglFormat, w, 0, texFrmt.GetSurfaceSize(w, 1), data);
                else
                    glCompressedTexSubImage1D(GL_TEXTURE_1D, oglLevel,
                                              lockInfo.xOffset, lockInfo.width,
                                              oglFormat, texFrmt.GetSurfaceSize(lockInfo.width, 1), data);
            } else {
                if (lockInfo.xOffset == 0 && lockInfo.yOffset == 0 && lockInfo.width == 0 && lockInfo.height == 0)
                    glTexImage1D(GL_TEXTURE_1D, oglLevel, oglFormat, w, 0, oglSrcFormat, oglSrcType, data);
                else
                    glTexSubImage1D(GL_TEXTURE_1D, oglLevel, lockInfo.xOffset, lockInfo.width, oglSrcFormat, oglSrcType, data);
            }
            break;

        case Texture2D:
            if (compressed) {
                if (lockInfo.xOffset == 0 && lockInfo.yOffset == 0 && lockInfo.width == 0 && lockInfo.height == 0)
                    glCompressedTexImage2D(GL_TEXTURE_2D, oglLevel, oglFormat, w, h, 0, texFrmt.GetSurfaceSize(w, h), data);
                else
                    glCompressedTexSubImage2D(GL_TEXTURE_2D, oglLevel,
                                              lockInfo.xOffset, lockInfo.yOffset, lockInfo.width, lockInfo.height,
                                              oglFormat, texFrmt.GetSurfaceSize(lockInfo.width, lockInfo.height), data);
            } else {
                if (lockInfo.xOffset == 0 && lockInfo.yOffset == 0 && lockInfo.width == 0 && lockInfo.height == 0)
                    glTexImage2D(GL_TEXTURE_2D, oglLevel, oglFormat, w, h, 0, oglSrcFormat, oglSrcType, data);
                else
                    glTexSubImage2D(GL_TEXTURE_2D, oglLevel,
                                    lockInfo.xOffset, lockInfo.yOffset, lockInfo.width, lockInfo.height,
                                    oglSrcFormat, oglSrcType, data);
            }
            break;

        case TextureCube:
            if (compressed) {
                if (lockInfo.xOffset == 0 && lockInfo.yOffset == 0 && lockInfo.width == 0 && lockInfo.height == 0)
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + lockInfo.cubeFace, oglLevel,
                                           oglFormat, w, h, 0, texFrmt.GetSurfaceSize(w, h), data);
                else
                    glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + lockInfo.cubeFace, oglLevel,
                                              lockInfo.xOffset, lockInfo.yOffset, lockInfo.width, lockInfo.height,
                                              oglFormat, texFrmt.GetSurfaceSize(lockInfo.width, lockInfo.height), data);
            } else {
                if (lockInfo.xOffset == 0 && lockInfo.yOffset == 0 && lockInfo.width == 0 && lockInfo.height == 0)
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + lockInfo.cubeFace, oglLevel,
                                 oglFormat, w, h, 0, oglSrcFormat, oglSrcType, data);
                else
                    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + lockInfo.cubeFace, oglLevel,
                                    lockInfo.xOffset, lockInfo.yOffset, lockInfo.width, lockInfo.height,
                                    oglSrcFormat, oglSrcType, data);
            }
            break;

        case Texture3D:
            assert(false);
            break;
    }

    if (generateMipmaps)
        glGenerateMipmap(oglTexType);

    CheckOGLErrors("TextureBuffer upload");
}

void
OGLTextureBuffer::Download(const LockInfo &lockInfo, void *data)
{
    BaseTextureBuffer::Download(lockInfo, data);

    GLenum oglTexType = ConvertTextureType(type);
    glBindTexture(oglTexType, id);

    GLint oglFormat,
          oglSrcType,
          oglSrcFormat;

    ConvertImageFormat(format, oglFormat, oglSrcType, oglSrcFormat);
    switch (type) {
        case Texture1D:
            glGetTexImage(GL_TEXTURE_1D, lockInfo.mipmapLevel, oglSrcFormat, oglSrcType, data);
            break;
        case Texture2D:
            glGetTexImage(GL_TEXTURE_2D, lockInfo.mipmapLevel, oglSrcFormat, oglSrcType, data);
            break;
        case TextureCube:
            glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + lockInfo.cubeFace, lockInfo.mipmapLevel, oglSrcFormat, oglSrcType, data);
            break;
        case Texture3D:
            assert(false);
            break;
    }

    CheckOGLErrors("TextureBuffer download");
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
