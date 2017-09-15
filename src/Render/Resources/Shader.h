#pragma once

#include "Core/SmartPtr.h"
#include "Core/Collections/Array_type.h"
#include "Render/Resources/Resource.h"
#include "Render/RenderModeState.h"
#include "Render/RenderObjects.h"

namespace Framework {

class Shader : public Resource {
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

    RenderModeState& GetRenderMode();
    const RenderModeState& GetRenderMode() const;

    const SmartPtr<RHI::ShaderProgram>& GetProgram() const;
};

inline RenderModeState&
Shader::GetRenderMode()
{
    return renderMode;
}

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
