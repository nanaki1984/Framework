#include "Render/Resources/ComputeShader.h"
#include "Core/Collections/Array.h"
#include "Core/Memory/ScratchAllocator.h"
#include "Core/Memory/BlocksAllocator.h"
#include "Core/IO/FileServer.h"
#include "Core/Log.h"
#include "Render/RenderQueue.h"
#include "Core/snprintf.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::ComputeShader, Framework::Resource);

ComputeShader::ComputeShader()
: srcCode(Memory::GetAllocator<ScratchAllocator>())
{ }

ComputeShader::~ComputeShader()
{ }

uint32_t
ComputeShader::GetAccessModes() const
{
    return ReadOnly;
}

bool
ComputeShader::parseFile(const char *filename, int depth, char *error, uint32_t errorSize)
{
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
            } else {
                snprintf(error, errorSize, "(%s,%d) Unknown pragma: %s", filename, lineCounter, ptr);
                return false;
            }
        } else {
            srcCode.Append(line);
        }
        ++lineCounter;
    }

    fclose(f);

    return true;
}

bool
ComputeShader::LoadImpl()
{
    char error[1024];
    if (!this->parseFile(filename.AsCString(), 0, error, sizeof(error))) {
        Log::Instance()->Write(Log::Error, "Shader parse error:\n%s", error);
        return false;
    }

    program = SmartPtr<RHI::ShaderProgram>::MakeNew<BlocksAllocator>();
    program->SetShaderSources(RHI::BaseShaderProgram::ComputeShader, std::move(srcCode));
    program->Link();

    srcCode.Clear();

    RenderQueue::Instance()->GetRenderer()->OnComputeShaderLoaded(this);

    return true;
}

void
ComputeShader::UnloadImpl()
{
    RenderQueue::Instance()->GetRenderer()->OnComputeShaderUnloaded(this);

    program.Reset();
}

bool
ComputeShader::CloneImpl(WeakPtr<Resource> source)
{
    return false;
}

uint32_t
ComputeShader::ComputeSize()
{
    return 0;
}

} // namespace Framework
