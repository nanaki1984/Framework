#pragma once

#include "Core/SmartPtr.h"
#include "Core/Collections/Array_type.h"
#include "Render/Resources/RenderResource.h"
#include "Render/RenderObjects.h"

namespace Framework {
        namespace RHI {

struct ComputeShaderRenderData : RenderData
{
    SmartPtr<RHI::ShaderProgram> program;

    ComputeShaderRenderData()
    { }
    ComputeShaderRenderData(ComputeShaderRenderData &&other)
    {
        program = std::forward<SmartPtr<RHI::ShaderProgram>>(other.program);
    }

    ComputeShaderRenderData& operator = (ComputeShaderRenderData &&other)
    {
        program = std::forward<SmartPtr<RHI::ShaderProgram>>(other.program);
        return (*this);
    }

    void Invalidate()
    {
        program.Reset();
    }
};

    } // namespace RHI

class ComputeShader : public RenderResource<RHI::ComputeShaderRenderData> {
    DeclareClassInfo;
protected:
    String srcCode;

    SmartPtr<RHI::ShaderProgram> program;

    bool parseFile(const char *filename, int depth, char *error, uint32_t errorSize);

    virtual bool LoadImpl();
    virtual void UnloadImpl();
    virtual bool CloneImpl(WeakPtr<Resource> source);
    virtual uint32_t ComputeSize();
public:
    ComputeShader();
    virtual ~ComputeShader();

    virtual uint32_t GetAccessModes() const;

    const SmartPtr<RHI::ShaderProgram>& GetProgram() const;

    bool PrepareForRendering(RenderQueue *renderQueue);
};

inline const SmartPtr<RHI::ShaderProgram>&
ComputeShader::GetProgram() const
{
    return program;
}

} // namespace Framework
