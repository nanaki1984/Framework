#pragma once

// Platform dependant GL includes
#ifdef EMSCRIPTEN
#   include "GL/glew.h"
#else
#   include "GL/glew.h"
#endif

#include "Core/Debug.h"
#include "Render/Image/ImageFormat.h"
#include "Render/RenderModeState.h"
#include "Render/Base/BaseTextureBuffer.h"
#include "Render/DrawPrimitives.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

void CheckErrors(const char *file, int line, const char *desc);
void ConvertImageFormat(ImageFormat::Type format, GLint &oglFormat, GLint &oglSrcType, GLint &oglSrcFormat);
GLenum ConvertCullMode(RenderModeState::CullMode cullMode);
GLenum ConvertCompareFunc(RenderModeState::CompareFunc compFunc);
GLenum ConvertBlendOp(RenderModeState::BlendOp blendOp);
GLenum ConvertBlendFactor(RenderModeState::BlendFactor blendFactor);
GLenum ConvertCubeFace(CubeFace cubeFace);
GLenum ConvertPrimitiveType(DrawPrimitives::PrimitiveType primType);
GLenum ConvertTextureType(BaseTextureBuffer::Type texType);

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework

#ifdef _DEBUG
#   define CheckOGLErrors(msg) Framework::RHI::OpenGL::CheckErrors(__FILE__, __LINE__, msg)
#else
#   define CheckOGLErrors(msg)
#endif // _DEBUG
