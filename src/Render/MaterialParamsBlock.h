#pragma once

#include "Render/Resources/Material.h"

namespace Framework {

class MaterialParamsBlock {
private:
    Array<Material::FloatParam>   floatParams;
    Array<Material::VectorParam>  vectorParams;
    Array<Material::MatrixParam>  matrixParams;
    Array<Material::TextureParam> textureParams;
	Array<Material::BufferParam>  bufferParams;
public:
    MaterialParamsBlock();
    MaterialParamsBlock(const MaterialParamsBlock &other);
    MaterialParamsBlock(MaterialParamsBlock &&other);
    ~MaterialParamsBlock();

    MaterialParamsBlock& operator =(const MaterialParamsBlock &other);
    MaterialParamsBlock& operator =(MaterialParamsBlock &&other);

    void Clear();

    void AddFloat(const StringHash &name, float value);
    const Material::FloatParam* FloatParamsBegin() const;
    const Material::FloatParam* FloatParamsEnd() const;

    void AddVector(const StringHash &name, const Math::Vector4 &value);
    const Material::VectorParam* VectorParamsBegin() const;
    const Material::VectorParam* VectorParamsEnd() const;

    void AddMatrix(const StringHash &name, const Math::Matrix &value);
    const Material::MatrixParam* MatrixParamsBegin() const;
    const Material::MatrixParam* MatrixParamsEnd() const;

    void AddTexture(const StringHash &name, const WeakPtr<Texture> &value);
    const Material::TextureParam* TextureParamsBegin() const;
    const Material::TextureParam* TextureParamsEnd() const;
    
    void AddBuffer(const StringHash &name, const SmartPtr<RHI::ComputeBuffer> &value);
	const Material::BufferParam* BufferParamsBegin() const;
	const Material::BufferParam* BufferParamsEnd() const;
};

} // namespace Framework
