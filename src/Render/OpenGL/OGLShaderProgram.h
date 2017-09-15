#pragma once

#include "Render/OpenGL/OpenGL.h"
#include "Render/Base/BaseShaderProgram.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

class OGLShaderProgram : public BaseShaderProgram
{
    DeclareClassInfo;
protected:
    GLuint oglProgHandle;
    GLuint oglShdHandles[ShadersCount];

    bool LoadShader(ShaderType shdType, int index);
public:
    OGLShaderProgram();
    virtual ~OGLShaderProgram();

    bool Link();

    GLuint GetProgramHandle() const;
};

inline GLuint
OGLShaderProgram::GetProgramHandle() const
{
    return oglProgHandle;
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
