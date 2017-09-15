#include "Render/OpenGL/OGLShaderProgram.h"
#include "Core/Log.h"

namespace Framework {
	namespace RHI {
		namespace OpenGL {

DefineClassInfo(Framework::RHI::OpenGL::OGLShaderProgram, Framework::RHI::BaseShaderProgram);

OGLShaderProgram::OGLShaderProgram()
{
    oglProgHandle = glCreateProgram();
    memset(oglShdHandles, 0, sizeof(GLuint) * ShadersCount);
}

OGLShaderProgram::~OGLShaderProgram()
{
    for (int i = 0; i < ShadersCount; ++i) {
        if (0 == oglShdHandles[i])
            continue;
        glDeleteShader(oglShdHandles[i]);
    }
    glDeleteProgram(oglProgHandle);
}

bool
OGLShaderProgram::LoadShader(ShaderType shdType, int index)
{
    if (sources[index].IsEmpty())
        return true;

    switch (shdType) {
        case BaseShaderProgram::VertexShader:
            oglShdHandles[index] = glCreateShader(GL_VERTEX_SHADER);
            break;
        case BaseShaderProgram::FragmentShader:
            oglShdHandles[index] = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case BaseShaderProgram::ComputeShader:
            oglShdHandles[index] = glCreateShader(GL_COMPUTE_SHADER);
            break;
        default:
            return false;
    }

    GLint result;
    char  info[256];
    int   infoLen;

    const char* sourceLine[1] = { sources[index].AsCString() };
    glShaderSource(oglShdHandles[index], 1, sourceLine, nullptr);
    glCompileShader(oglShdHandles[index]);
    glGetShaderiv(oglShdHandles[index], GL_COMPILE_STATUS, &result);
    glGetShaderInfoLog(oglShdHandles[index], 256, &infoLen, info);

    if (result) {
        return true;
    } else {
        Log::Instance()->Write(Log::Error, "OpenGL shader compile error: %s", info);
        return false;
    }
}

bool
OGLShaderProgram::Link()
{
    assert(!isLinked);

    bool shadersOk = true;
    if (isComputeShader)
    {
        shadersOk = this->LoadShader(ComputeShader, 0);

        if (shadersOk)
            glAttachShader(oglProgHandle, oglShdHandles[0]);
    }
    else
    {
        for (int i = 0; i < ShadersCount; ++i) {
            shadersOk &= this->LoadShader(static_cast<ShaderType>(i), i);
            if (!shadersOk)
                break;

            if (0 == oglShdHandles[i])
                continue;

            glAttachShader(oglProgHandle, oglShdHandles[i]);
        }
    }

    if (!shadersOk)
        return BaseShaderProgram::Link();

    GLint result;
    char  info[1024];
    int   infoLen;

    glLinkProgram(oglProgHandle);
    glGetProgramiv(oglProgHandle, GL_LINK_STATUS, &result);
    glGetProgramInfoLog(oglProgHandle, 1024, &infoLen, info);

    if (0 == result) {
        Log::Instance()->Write(Log::Error, "OpenGL shaders link error: %s", info);

        isLinked = false;
    } else {
        Log::Instance()->Write(Log::Info, "OpenGL shaders link info: %s", info);

        isLinked = true;
    }

    if (isLinked) {
        char *name = info;
        if (!isComputeShader)
        {
            // set attributes
            GLint nAttrib, attribIndex = 0;
            glGetProgramiv(oglProgHandle, GL_ACTIVE_ATTRIBUTES, &nAttrib);

            for (int j = 0; j < nAttrib; ++j) {
                GLenum attribType;
                GLint  length, size;

                glGetActiveAttrib(oglProgHandle, j, 1024, &length, &size, &attribType, name);

                // check prefix gl_
                const char* c = name;
                if ('g' == *(c++) && 'l' == *(c++) && '_' == *(c++)) continue;

                glBindAttribLocation(oglProgHandle, attribIndex, name);
                this->AddParam(name, size, attribType, attribIndex++);
            }
        }

        // re-link program to apply attributes bindings...
        glLinkProgram(oglProgHandle);

        // set samplers AFTER re-link
        glUseProgram(oglProgHandle);

        GLint nUniforms, nSamplers = 0;
	    glGetProgramiv(oglProgHandle, GL_ACTIVE_UNIFORMS, &nUniforms);

	    for (int i = 0; i < nUniforms; ++i) {
		    GLenum uniformType;
		    GLint  length, size;

            glGetActiveUniform(oglProgHandle, i, 1024, &length, &size, &uniformType, name);

		    // enumerate all samplers
		    if (uniformType >= GL_SAMPLER_1D && uniformType <= GL_SAMPLER_2D_RECT_SHADOW) {
			    GLint location = glGetUniformLocation(oglProgHandle, name);

                glUniform1i(location, nSamplers);
                this->AddParam(name, size, uniformType, nSamplers++);
            } else {
                this->AddParam(name, size, uniformType, glGetUniformLocation(oglProgHandle, name));
            }

            // ToDo: only for compute shader check for image samplers (GL_IMAGE_2D)
	    }

        if (isComputeShader)
        {
            GLint nBuffers;
            glGetProgramInterfaceiv(oglProgHandle, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &nBuffers);
            
            for (int i = 0; i < nBuffers; ++i)
            {
                GLint length;
                glGetProgramResourceName(oglProgHandle, GL_SHADER_STORAGE_BLOCK, i, 1024, &length, name);

                this->AddParam(name, 0, GL_SHADER_STORAGE_BUFFER, glGetProgramResourceIndex(oglProgHandle, GL_SHADER_STORAGE_BLOCK, name));
            }

            glGetProgramiv(oglProgHandle, GL_COMPUTE_WORK_GROUP_SIZE, workGroupSizes);
        }

        glUseProgram(0);

#ifdef _DEBUG
        // validate program
        glValidateProgram(oglProgHandle);
        glGetProgramiv(oglProgHandle, GL_VALIDATE_STATUS, &result);
        assert(GL_TRUE == result);
        glGetProgramInfoLog(oglProgHandle, 1024, &infoLen, info);
        Log::Instance()->Write(Log::Info, "OpenGL shaders validation info: %s", info);
#endif // _DEBUG

        CheckOGLErrors("ShaderProgram:Link");
    }

    return BaseShaderProgram::Link();
}

		} // namespace OpenGL
	} // namespace RHI
} // namespace Framework
