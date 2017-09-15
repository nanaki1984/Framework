#include "Render/OpenGL/OpenGL.h"
#include "Core/Log.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

static GLint oglFormats[] = {
    0,                             //  ? bit
    
    // image formats
    GL_INTENSITY8,                 //  8 bit
    GL_LUMINANCE8_ALPHA8,          // 16 bit
    GL_RGB5,                       // 16 bit
    GL_RGB5_A1,                    // 16 bit
    GL_RGBA4,                      // 16 bit
    GL_INTENSITY16,                // 16 bit
    GL_INTENSITY_FLOAT16_ATI,      // 16 bit
    GL_RGB10_A2,                   // 32 bit
    GL_LUMINANCE16_ALPHA16,        // 32 bit
    GL_RGBA8,                      // 32 bit GL_RGB8
    GL_RGBA8,                      // 32 bit
    GL_LUMINANCE_ALPHA_FLOAT16_ATI,// 32 bit
    GL_INTENSITY_FLOAT32_ATI,      // 32 bit
    GL_LUMINANCE_ALPHA_FLOAT32_ATI,// 64 bit
    GL_RGBA_FLOAT16_ATI,           // 64 bit
    GL_RGBA_FLOAT32_ATI,           //128 bit

    // compressed image formats
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

    // depth stencil formats (not used in opengl)
    0,
    0,
    0,
    0,
    0
};

static GLint oglSrcTypes[] = {
    0,                             //  ? bit
    
    // image formats
    GL_UNSIGNED_BYTE,              //  8 bit
    GL_UNSIGNED_BYTE,              // 16 bit
    GL_UNSIGNED_SHORT_5_6_5_REV,   // 16 bit
    GL_UNSIGNED_SHORT_1_5_5_5_REV, // 16 bit
    GL_UNSIGNED_SHORT_4_4_4_4_REV, // 16 bit
    GL_UNSIGNED_SHORT,             // 16 bit
    GL_HALF_FLOAT,                 // 16 bit
    GL_UNSIGNED_INT_10_10_10_2,    // 32 bit
    GL_UNSIGNED_SHORT,             // 32 bit
    GL_UNSIGNED_BYTE,              // 32 bit
    GL_UNSIGNED_BYTE,              // 32 bit
    GL_HALF_FLOAT,                 // 32 bit
    GL_FLOAT,                      // 32 bit
    GL_FLOAT,                      // 64 bit
    GL_HALF_FLOAT,                 // 64 bit
    GL_FLOAT,                      //128 bit

    // compressed image formats
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,

    // depth stencil formats (not used in opengl)
    0,
    0,
    0,
    0,
    0
};

static GLint oglSrcFormats[] = {
    0,                  //  ? bit
    
    // image formats
    GL_LUMINANCE,       //  8 bit
    GL_LUMINANCE_ALPHA, // 16 bit
    GL_RGB,             // 16 bit
    GL_RGBA,            // 16 bit
    GL_RGBA,            // 16 bit
    GL_LUMINANCE,       // 16 bit
    GL_LUMINANCE,       // 16 bit
    GL_RGBA,            // 32 bit
    GL_LUMINANCE_ALPHA, // 32 bit
    GL_RGBA,            // 32 bit GL_RGB
    GL_RGBA,            // 32 bit
    GL_LUMINANCE_ALPHA, // 32 bit
    GL_LUMINANCE,       // 32 bit
    GL_LUMINANCE_ALPHA, // 64 bit
    GL_RGBA,            // 64 bit
    GL_RGBA,            //128 bit
    
    // compressed image formats
    GL_RGB,
    GL_RGBA,
    GL_RGBA,
    
    // depth stencil formats (not used in opengl)
    0,
    0,
    0,
    0,
    0
};

static GLenum oglCullModes[] = {
    0,       // call glDisable ( GL_CULL_FACE )
    GL_FRONT,
    GL_BACK
};

static GLenum oglCompFuncs[] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

static GLenum oglBlendOps[] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX
};

static GLenum oglBlendFactors[] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA_SATURATE
};

static GLenum oglCubeFaces[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

static GLenum oglPrimTypes[] = {
    0,
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN
};

static GLenum oglTexTypes[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_3D
};

void
CheckErrors(const char *file, int line, const char *desc)
{
	GLuint err;
	do
	{
		err = glGetError();
		if (0 != err) {
			Log::Instance()->Write(Log::Warning, "OpenGL error (%s, %d): %X, desc: %s", file, line, err, desc);
			assert(false);
		}
	} while (err != 0);
}

void
ConvertImageFormat(ImageFormat::Type format, GLint &oglFormat, GLint &oglSrcType, GLint &oglSrcFormat)
{
    oglFormat    = oglFormats[format];
    oglSrcType   = oglSrcTypes[format];
    oglSrcFormat = oglSrcFormats[format];
}

GLenum
ConvertCullMode(RenderModeState::CullMode cullMode)
{
    return oglCullModes[cullMode];
}

GLenum
ConvertCompareFunc(RenderModeState::CompareFunc compFunc)
{
    return oglCompFuncs[compFunc];
}

GLenum
ConvertBlendOp(RenderModeState::BlendOp blendOp)
{
    return oglBlendOps[blendOp];
}

GLenum
ConvertBlendFactor(RenderModeState::BlendFactor blendFactor)
{
    return oglBlendFactors[blendFactor];
}

GLenum
ConvertCubeFace(CubeFace cubeFace)
{
    return oglCubeFaces[cubeFace];
}

GLenum
ConvertPrimitiveType(DrawPrimitives::PrimitiveType primType)
{
    return oglPrimTypes[primType];
}

GLenum
ConvertTextureType(BaseTextureBuffer::Type texType)
{
    return oglTexTypes[texType];
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
