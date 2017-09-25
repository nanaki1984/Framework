#include "Render/Base/BaseShaderProgram.h"
#include "Core/Collections/Hash.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/BlocksAllocator.h"

namespace Framework {
    namespace RHI {

DefineClassInfo(Framework::RHI::BaseShaderProgram, Framework::RefCounted);

BaseShaderProgram::BaseShaderProgram()
: params(Memory::GetAllocator<MallocAllocator>()),
  isLinked(false),
  isComputeShader(false)
{
    Memory::Zero(workGroupSizes, 3);
}

BaseShaderProgram::~BaseShaderProgram()
{ }

void
BaseShaderProgram::AddParam(const char *name, uint32_t size, uint32_t type, uint32_t regOrIndex)
{
    ShaderParam param;

    param.name = StringHash::FromCString(name);
    param.size = size;
    param.type = type;
    param.regOrIndex = regOrIndex;

    assert(!this->HasParam(param.name));

    params.Add(param.name.hash, param);
}

void
BaseShaderProgram::SetShaderSources(ShaderType shdType, String &&shdSources)
{
    assert(shdType < ShadersCount || ComputeShader == shdType);
    if (ComputeShader == shdType)
    {
        isComputeShader = true;
        sources[0] = std::forward<String>(shdSources);
    }
    else
    {
        assert(!isComputeShader);
        sources[shdType] = std::forward<String>(shdSources);
    }
}

const char*
BaseShaderProgram::GetShaderSources(ShaderType shdType) const
{
    assert(shdType < ShadersCount || ComputeShader == shdType);
    return sources[ComputeShader == shdType ? 0 : shdType].AsCString();
}

void
BaseShaderProgram::ClearShaderSources(ShaderType shdType)
{
    assert(shdType < ShadersCount || ComputeShader == shdType);
    if (ComputeShader == shdType)
    {
        isComputeShader = false;
        sources[0] = String();
    }
    else
    {
        assert(!isComputeShader);
        sources[shdType] = String();
    }
}

bool
BaseShaderProgram::Link()
{
    if (isLinked) {
        for (int i = 0; i < ShadersCount; ++i)
            sources[i] = String();
    }
    return isLinked;
}

bool
BaseShaderProgram::HasParam(const StringHash &name) const
{
    const ShaderParam *param = params.Get(name.hash);
    while (param != nullptr) {
        if (param->name == name)
            break;
        param = params.Next(param);
    }
    return param != nullptr;
}

const BaseShaderProgram::ShaderParam&
BaseShaderProgram::GetParam(const StringHash &name) const
{
    const ShaderParam *param = params.Get(name.hash);
    while (param != nullptr) {
        if (param->name == name)
            break;
        param = params.Next(param);
    }
    assert(param != nullptr);
    return *param;
}

bool
BaseShaderProgram::TryGetParam(const StringHash &name, ShaderParam& outParam) const
{
    const ShaderParam *param = params.Get(name.hash);
    while (param != nullptr) {
        if (param->name == name)
            break;
        param = params.Next(param);
    }
    if (nullptr == param)
        return false;
    else {
        outParam.name = param->name;
        outParam.size = param->size;
        outParam.type = param->type;
        outParam.regOrIndex = param->regOrIndex;

        return true;
    }
}

void
BaseShaderProgram::GetWorkGroupSizes(int &x, int &y, int &z)
{
    assert(isLinked && isComputeShader);
    x = workGroupSizes[0];
    y = workGroupSizes[1];
    z = workGroupSizes[2];
}

    } // namespace RHI
} // namespace Framework

DefineEnumStrings(Framework::RHI::BaseShaderProgram::Translucency) = {
    "Opaque",
    "AlphaMul",
    "AlphaAdd",
    "AlphaBlend"
};
