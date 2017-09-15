#include "Render/Resources/Material.h"
#include "Render/Resources/ResourceServer.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Log.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::Material, Framework::Resource);

Material::Material()
: floatParams (Memory::GetAllocator<MallocAllocator>()),
  vectorParams(Memory::GetAllocator<MallocAllocator>()),
  matrixParams(Memory::GetAllocator<MallocAllocator>()),
  textures    (Memory::GetAllocator<MallocAllocator>())
{ }

Material::~Material()
{ }

uint32_t
Material::GetAccessModes() const
{
    return ReadOnly | Writable;
}

void
Material::SetShader(const char *filename)
{
	shader = ResourceServer::Instance()->NewResourceFromFile<Shader>(filename, ReadOnly);
}

void
Material::CopyParamsFrom(const WeakPtr<Material> &otherMaterial)
{
	floatParams  = otherMaterial->floatParams;
	vectorParams = otherMaterial->vectorParams;
	matrixParams = otherMaterial->matrixParams;
	textures     = otherMaterial->textures;
}

bool
Material::LoadImpl()
{
    return true;
}

void
Material::UnloadImpl()
{
    shader.Invalidate();

    floatParams  = Hash<FloatParam>  (Memory::GetAllocator<MallocAllocator>());
    vectorParams = Hash<VectorParam> (Memory::GetAllocator<MallocAllocator>());
    matrixParams = Hash<MatrixParam> (Memory::GetAllocator<MallocAllocator>());
    textures     = Hash<TextureParam>(Memory::GetAllocator<MallocAllocator>());
}

bool
Material::CloneImpl(WeakPtr<Resource> source)
{
    Material *mat = source.Cast<Material>();

    shader = mat->shader;

    floatParams  = mat->floatParams;
    vectorParams = mat->vectorParams;
    matrixParams = mat->matrixParams;
    textures     = mat->textures;

    return true;
}

uint32_t
Material::ComputeSize()
{
    return 0;
}

float
Material::GetFloat(const StringHash &name) const
{
    return GetParam(floatParams, name);
}

void
Material::SetFloat(const StringHash &name, float value)
{
    SetParam(floatParams, name, value);
}

const Material::FloatParam*
Material::FloatParamsBegin() const
{
    return floatParams.Begin();
}

const Material::FloatParam*
Material::FloatParamsEnd() const
{
    return floatParams.End();
}

Math::Vector4
Material::GetVector(const StringHash &name) const
{
    return GetParam(vectorParams, name);
}

void
Material::SetVector(const StringHash &name, const Math::Vector4 &value)
{
    SetParam(vectorParams, name, value);
}

const Material::VectorParam*
Material::VectorParamsBegin() const
{
    return vectorParams.Begin();
}

const Material::VectorParam*
Material::VectorParamsEnd() const
{
    return vectorParams.End();
}

Math::Matrix
Material::GetMatrix(const StringHash &name) const
{
    return GetParam(matrixParams, name).GetTransposed();
}

void
Material::SetMatrix(const StringHash &name, const Math::Matrix &value)
{
    SetParam(matrixParams, name, value.GetTransposed());
}

const Material::MatrixParam*
Material::MatrixParamsBegin() const
{
    return matrixParams.Begin();
}

const Material::MatrixParam*
Material::MatrixParamsEnd() const
{
    return matrixParams.End();
}

WeakPtr<Texture>
Material::GetTexture(const StringHash &name) const
{
    return GetParam(textures, name);
}

void
Material::SetTexture(const StringHash &name, const WeakPtr<Texture> &value)
{
    SetParam(textures, name, value);
}

void
Material::SetTexture(const StringHash &name, const char *filename, Access access)
{
	SetParam(textures, name, ResourceServer::Instance()->NewResourceFromFile<Texture>(filename, access));
}

const Material::TextureParam*
Material::TextureParamsBegin() const
{
    return textures.Begin();
}

const Material::TextureParam*
Material::TextureParamsEnd() const
{
    return textures.End();
}

} // namespace Framework
