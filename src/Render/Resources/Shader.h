#pragma once

#include "Core/SmartPtr.h"
#include "Core/Collections/Array_type.h"
#include "Render/Resources/RenderResource.h"
#include "Render/RenderModeState.h"
#include "Render/RenderObjects.h"

namespace Framework {
    namespace RHI {

struct ShaderRenderData : RenderData
{
    SmartPtr<RHI::ShaderProgram> program;

    ShaderRenderData()
    { }
    ShaderRenderData(ShaderRenderData &&other)
    {
        program = std::forward<SmartPtr<RHI::ShaderProgram>>(other.program);
    }

    ShaderRenderData& operator = (ShaderRenderData &&other)
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

class Shader : public RenderResource<RHI::ShaderRenderData> {
    DeclareClassInfo;
protected:
    Array<String> srcLines;
    int32_t startLines[RHI::BaseShaderProgram::ShadersCount];

    RenderModeState renderMode;
    SmartPtr<RHI::ShaderProgram> program; // ToDo: passes

    bool parseFile(const char *filename, int depth, char *error, uint32_t errorSize);

    virtual bool LoadImpl();
    virtual void UnloadImpl();
    virtual bool CloneImpl(WeakPtr<Resource> source);
    virtual uint32_t ComputeSize();
public:
    Shader();
    virtual ~Shader();

    virtual uint32_t GetAccessModes() const;

    const RenderModeState& GetRenderMode() const;

    const SmartPtr<RHI::ShaderProgram>& GetProgram() const;

    bool PrepareForRendering(RenderQueue *renderQueue);
};

inline const RenderModeState&
Shader::GetRenderMode() const
{
    return renderMode;
}

inline const SmartPtr<RHI::ShaderProgram>&
Shader::GetProgram() const
{
    return program;
}

} // namespace Framework
