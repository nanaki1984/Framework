#pragma once

#include "Core/RefCounted.h"
#include "Core/StringHash.h"
#include "Core/String.h"
#include "Core/Collections/Hash_type.h"

namespace Framework {
    namespace RHI {

class BaseShaderProgram : public RefCounted {
    DeclareClassInfo;
public:
    enum ShaderType {
        VertexShader = 0,
        FragmentShader,

        ShadersCount,

        ComputeShader = 0xff
    };

    enum Translucency {
        Opaque = 0,
        AlphaMul,
        AlphaAdd,
        AlphaBlend
    };

    struct ShaderParam {
        StringHash name;
        uint32_t size; // API dependant
        uint32_t type; // API dependant
        uint32_t regOrIndex;
    };
protected:
    Translucency translucency;

    String sources[ShadersCount];

    Hash<ShaderParam> params;

    int workGroupSizes[3];

    bool isLinked;
    bool isComputeShader;

    void AddParam(const char *name, uint32_t size, uint32_t type, uint32_t regOrIndex);
public:
    BaseShaderProgram();
    virtual ~BaseShaderProgram();

    Translucency GetTranslucency() const;
    void SetTranslucency(Translucency _translucency);

    void SetShaderSources(ShaderType shdType, String &&shdSources);
    const char* GetShaderSources(ShaderType shdType) const;
    void ClearShaderSources(ShaderType shdType);

    bool IsLinked() const;
    bool Link();

    bool HasParam(const StringHash &name) const;
    const ShaderParam& GetParam(const StringHash &name) const;
    bool TryGetParam(const StringHash &name, ShaderParam& outParam) const;

    void GetWorkGroupSizes(int &x, int &y, int &z);
};

inline BaseShaderProgram::Translucency
BaseShaderProgram::GetTranslucency() const
{
    return translucency;
}

inline void
BaseShaderProgram::SetTranslucency(Translucency _translucency)
{
    translucency = _translucency;
}

inline bool
BaseShaderProgram::IsLinked() const
{
    return isLinked;
}

    } // namespace RHI
} // namespace Framework
