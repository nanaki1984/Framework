#pragma once

#include "Render/Resources/RenderResource.h"
#include "Render/Resources/Shader.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Core/Collections/Hash.h"
#include "Render/MaterialParamsBlock.h"

namespace Framework {

    namespace RHI {

struct MaterialRenderData
{
    RenderModeState     renderMode;
    MaterialParamsBlock parameters;

    MaterialRenderData()
    { }
    MaterialRenderData(MaterialRenderData &&other)
    {
        renderMode = other.renderMode;
        parameters = std::forward<MaterialParamsBlock>(other.parameters);
    }

    MaterialRenderData& operator = (MaterialRenderData &&other)
    {
        renderMode = other.renderMode;
        parameters = std::forward<MaterialParamsBlock>(other.parameters);
        return (*this);
    }

    void Invalidate()
    {
        parameters.Clear();
    }
};

    } // namespace RHI

class Material : public RenderResource<RHI::MaterialRenderData> {
    DeclareClassInfo;
protected:
    WeakPtr<Shader> shader;

    Hash<Materials::FloatParam>   floatParams;
    Hash<Materials::VectorParam>  vectorParams;
    Hash<Materials::MatrixParam>  matrixParams;
    Hash<Materials::TextureParam> textures;

    RenderModeState renderMode;

    template <typename T>
    static
    T GetParam(const Hash<Materials::Param<T>> &hash, const StringHash &name)
    {
        const Materials::Param<T> *item = hash.Get(name.hash);
        while (item != nullptr) {
            if (item->name == name)
                break;
            item = hash.Next(item);
        }
        return (nullptr == item ? T() : item->value);
    }

    template <typename T>
    static
    void SetParam(Hash<Materials::Param<T>> &hash, const StringHash &name, const T &value)
    {
        Materials::Param<T> *item = hash.Get(name.hash);
        while (item != nullptr) {
            if (item->name == name)
                break;
            item = hash.Next(item);
        }
        if (nullptr == item) {
            Materials::Param<T> param;
            param.name = name;
            param.value = value;
            hash.Add(name.hash, param);
        } else {
            item->value = value;
        }
    }

    virtual bool LoadImpl();
    virtual void UnloadImpl();
    virtual bool CloneImpl(WeakPtr<Resource> source);
    virtual uint32_t ComputeSize();
public:
    Material();
    virtual ~Material();

    virtual uint32_t GetAccessModes() const;

    const WeakPtr<Shader>& GetShader() const;
    void SetShader(const WeakPtr<Shader> &newShader);
	void SetShader(const char *filename);

    RenderModeState& GetRenderMode();
    const RenderModeState& GetRenderMode() const;

    void CopyParamsFrom(const WeakPtr<Material> &otherMaterial);

    float GetFloat(const StringHash &name) const;
    void SetFloat(const StringHash &name, float value);
    const Materials::FloatParam* FloatParamsBegin() const;
    const Materials::FloatParam* FloatParamsEnd() const;

    Math::Vector4 GetVector(const StringHash &name) const;
    void SetVector(const StringHash &name, const Math::Vector4 &value);
    const Materials::VectorParam* VectorParamsBegin() const;
    const Materials::VectorParam* VectorParamsEnd() const;

    Math::Matrix GetMatrix(const StringHash &name) const;
    void SetMatrix(const StringHash &name, const Math::Matrix &value);
    const Materials::MatrixParam* MatrixParamsBegin() const;
    const Materials::MatrixParam* MatrixParamsEnd() const;

    WeakPtr<Texture> GetTexture(const StringHash &name) const;
    void SetTexture(const StringHash &name, const WeakPtr<Texture> &value);
	void SetTexture(const StringHash &name, const char *filename, Access access = ReadOnly);
    const Materials::TextureParam* TextureParamsBegin() const;
    const Materials::TextureParam* TextureParamsEnd() const;

    bool PrepareForRendering(RenderQueue *renderQueue);
};

inline const WeakPtr<Shader>&
Material::GetShader() const
{
    return shader;
}

inline void
Material::SetShader(const WeakPtr<Shader> &newShader)
{
    shader = newShader;
    renderMode = newShader->GetRenderMode();
}

inline RenderModeState&
Material::GetRenderMode()
{
    return renderMode;
}

inline const RenderModeState&
Material::GetRenderMode() const
{
    return renderMode;
}

} // namespace Framework
