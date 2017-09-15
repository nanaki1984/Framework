#pragma once

#include "Render/Resources/Resource.h"
#include "Render/Resources/Texture.h"
#include "Render/Resources/Shader.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Core/Collections/Hash.h"

namespace Framework {

class Material : public Resource {
    DeclareClassInfo;
public:
    template <typename T>
    struct Param {
        StringHash name;
        T		   value;
    };

    typedef Param<float>		                FloatParam;
    typedef Param<Math::Vector4>                VectorParam;
    typedef Param<Math::Matrix>                 MatrixParam;
    typedef Param<WeakPtr<Texture>>             TextureParam;
    typedef Param<SmartPtr<RHI::ComputeBuffer>> BufferParam;
protected:
    WeakPtr<Shader> shader;

    Hash<Param<float>> floatParams;
    Hash<Param<Math::Vector4>> vectorParams;
    Hash<Param<Math::Matrix>> matrixParams;
    Hash<Param<WeakPtr<Texture>>> textures;

    template <typename T>
    static
    T GetParam(const Hash<Param<T>> &hash, const StringHash &name)
    {
        const Param<T> *item = hash.Get(name.hash);
        while (item != nullptr) {
            if (item->name == name)
                break;
            item = hash.Next(item);
        }
        return (nullptr == item ? T() : item->value);
    }

    template <typename T>
    static
    void SetParam(Hash<Param<T>> &hash, const StringHash &name, const T &value)
    {
        Param<T> *item = hash.Get(name.hash);
        while (item != nullptr) {
            if (item->name == name)
                break;
            item = hash.Next(item);
        }
        if (nullptr == item) {
            Param<T> param;
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

    void CopyParamsFrom(const WeakPtr<Material> &otherMaterial);

    float GetFloat(const StringHash &name) const;
    void SetFloat(const StringHash &name, float value);
    const FloatParam* FloatParamsBegin() const;
    const FloatParam* FloatParamsEnd() const;

    Math::Vector4 GetVector(const StringHash &name) const;
    void SetVector(const StringHash &name, const Math::Vector4 &value);
    const VectorParam* VectorParamsBegin() const;
    const VectorParam* VectorParamsEnd() const;

    Math::Matrix GetMatrix(const StringHash &name) const;
    void SetMatrix(const StringHash &name, const Math::Matrix &value);
    const MatrixParam* MatrixParamsBegin() const;
    const MatrixParam* MatrixParamsEnd() const;

    WeakPtr<Texture> GetTexture(const StringHash &name) const;
    void SetTexture(const StringHash &name, const WeakPtr<Texture> &value);
	void SetTexture(const StringHash &name, const char *filename, Access access = ReadOnly);
    const TextureParam* TextureParamsBegin() const;
    const TextureParam* TextureParamsEnd() const;
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
}

} // namespace Framework
