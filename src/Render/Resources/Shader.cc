#include "Render/Resources/Shader.h"
#include "Core/Collections/Array.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/BlocksAllocator.h"
#include "Core/IO/FileServer.h"
#include "Core/Log.h"
#include "Render/RenderQueue.h"
#include "Core/snprintf.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::Shader, Framework::Resource);

Shader::Shader()
: srcLines(Memory::GetAllocator<MallocAllocator>())
{ }

Shader::~Shader()
{ }

uint32_t
Shader::GetAccessModes() const
{
    return ReadOnly;
}

bool
Shader::parseFile(const char *filename, int depth, char *error, uint32_t errorSize)
{ // ToDo: passes
    if (depth > 8) {
        snprintf(error, errorSize, "(%s,0) Reached max depth", filename);
        return false;
    }

    String str = FileServer::Instance()->ResolvePath(filename);
    FILE *f = fopen(str.AsCString(), "r");
    if (nullptr == f) {
        snprintf(error, errorSize, "Cound't open file \"%s\"", str.AsCString());
        return false;
    }

    char line[1024];
    int lineCounter = 0;
    while (fgets(line, 1024, f)) {
        if (line == strstr(line, "#pragma ")) {
            size_t lineLen = strlen(line);
            assert(lineLen > 0);
            line[lineLen - 1] = '\0';

            char *ptr = line + 8;
            if (ptr == strstr(ptr, "include \"")) {
                // include file
                ptr += 9;
                char *endPtr = strchr(ptr, '\"');
                if (nullptr == endPtr)
                    return false;
                else {
                    String path("shaders_include:");
                    while (ptr < endPtr)
                        path += *(ptr++);
                    if (!this->parseFile(FileServer::Instance()->ResolvePath(path).AsCString(), depth + 1, error, errorSize))
                        return false;
                }
            } else if (ptr == strstr(ptr, "vertex")) {
                // vertex shader start
                if (startLines[RHI::BaseShaderProgram::VertexShader] > -1) {
                    snprintf(error, errorSize, "(%s,%d) Vertex Shader already present", filename, lineCounter);
                    return false;
                }
                startLines[RHI::BaseShaderProgram::VertexShader] = srcLines.Count();
            } else if (ptr == strstr(ptr, "fragment")) {
                // fragment shader start
                if (startLines[RHI::BaseShaderProgram::FragmentShader] > -1) {
                    snprintf(error, errorSize, "(%s,%d) Fragment Shader already present", filename, lineCounter);
                    return false;
                }
                startLines[RHI::BaseShaderProgram::FragmentShader] = srcLines.Count();
            } else {
                // renderMode
                if (ptr == strstr(ptr, "cullMode ")) {
                    ptr += 9;
                    renderMode.cullMode = StringToEnum<RenderModeState::CullMode>(ptr);
                } else if (ptr == strstr(ptr, "colorMask ")) {
                    ptr += 10;
                    renderMode.colorMask = StringToEnum<RenderModeState::ColorMask>(ptr);
                } else if (ptr == strstr(ptr, "zEnable ")) {
                    ptr += 8;
                    renderMode.zEnable = *ptr != '0';
                } else if (ptr == strstr(ptr, "zWriteEnable ")) {
                    ptr += 13;
                    renderMode.zWriteEnable = *ptr != '0';
                } else if (ptr == strstr(ptr, "zFunc ")) {
                    ptr += 6;
                    renderMode.zFunc = StringToEnum<RenderModeState::CompareFunc>(ptr);
                } else if (ptr == strstr(ptr, "blendEnable ")) {
                    ptr += 12;
                    renderMode.zWriteEnable = *ptr != '0';
                } else if (ptr == strstr(ptr, "blendOp ")) {
                    ptr += 8;
                    renderMode.blendOp = StringToEnum<RenderModeState::BlendOp>(ptr);
                } else if (ptr == strstr(ptr, "blendSrcFactor ")) {
                    ptr += 15;
                    renderMode.blendSrcFactor = StringToEnum<RenderModeState::BlendFactor>(ptr);
                } else if (ptr == strstr(ptr, "blendDstFactor ")) {
                    ptr += 15;
                    renderMode.blendDstFactor = StringToEnum<RenderModeState::BlendFactor>(ptr);
                } else {
                    snprintf(error, errorSize, "(%s,%d) Unknown pragma: %s", filename, lineCounter, ptr);
                    return false;
                }
            }
        } else {
            srcLines.PushBack(line);
        }
        ++lineCounter;
    }

    fclose(f);

    return true;
}

bool
Shader::LoadImpl()
{
    int i = 0;
    for (; i < RHI::BaseShaderProgram::ShadersCount; ++i)
        startLines[i] = -1;

    char error[1024];
    if (!this->parseFile(filename.AsCString(), 0, error, sizeof(error))) {
        Log::Instance()->Write(Log::Error, "Shader parse error:\n%s", error);
        return false;
    }

    program = SmartPtr<RHI::ShaderProgram>::MakeNew<BlocksAllocator>();

    for (i = 0; i < RHI::BaseShaderProgram::ShadersCount; ++i) {
        int32_t startLine = startLines[i];
        if (startLine < 0)
            continue;

        int32_t endLine = srcLines.Count();
        int offset = RHI::BaseShaderProgram::ShadersCount - 1;
        do {
            int32_t l = startLines[(i + offset--) % RHI::BaseShaderProgram::ShadersCount];
            if (l > startLine && l < endLine)
                endLine = l;
        } while (offset > 0);

        String str;
        for (int j = startLine; j < endLine; ++j)
            str += srcLines[j];

        program->SetShaderSources(static_cast<RHI::BaseShaderProgram::ShaderType>(i), std::move(str));
    }

    srcLines.SetCapacity(0);

    program->Link();

    RenderQueue::Instance()->GetRenderer()->OnShaderLoaded(this);

    return true;
}

void
Shader::UnloadImpl()
{
    RenderQueue::Instance()->GetRenderer()->OnShaderUnloaded(this);

    program.Reset();
}

bool
Shader::CloneImpl(WeakPtr<Resource> source)
{
    return false;
}

uint32_t
Shader::ComputeSize()
{
    return 0;
}

} // namespace Framework
