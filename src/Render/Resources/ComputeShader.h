#pragma once

#include "Core/SmartPtr.h"
#include "Core/Collections/Array_type.h"
#include "Render/Resources/Resource.h"
#include "Render/RenderObjects.h"

namespace Framework {

class ComputeShader : public Resource {
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
};

inline const SmartPtr<RHI::ShaderProgram>&
ComputeShader::GetProgram() const
{
    return program;
}

} // namespace Framework
