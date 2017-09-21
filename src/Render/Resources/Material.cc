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
    renderMode = shader->GetRenderMode();
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

    floatParams  = Hash<Materials::FloatParam>  (Memory::GetAllocator<MallocAllocator>());
    vectorParams = Hash<Materials::VectorParam> (Memory::GetAllocator<MallocAllocator>());
    matrixParams = Hash<Materials::MatrixParam> (Memory::GetAllocator<MallocAllocator>());
    textures     = Hash<Materials::TextureParam>(Memory::GetAllocator<MallocAllocator>());
}

bool
Material::CloneImpl(WeakPtr<Resource> source)
{
    Material *mat = source.Cast<Material>();

    shader = mat->shader;
    renderMode = mat->renderMode;

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

const Materials::FloatParam*
Material::FloatParamsBegin() const
{
    return floatParams.Begin();
}

const Materials::FloatParam*
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

const Materials::VectorParam*
Material::VectorParamsBegin() const
{
    return vectorParams.Begin();
}

const Materials::VectorParam*
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

const Materials::MatrixParam*
Material::MatrixParamsBegin() const
{
    return matrixParams.Begin();
}

const Materials::MatrixParam*
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

const Materials::TextureParam*
Material::TextureParamsBegin() const
{
    return textures.Begin();
}

const Materials::TextureParam*
Material::TextureParamsEnd() const
{
    return textures.End();
}

bool
Material::PrepareForRendering(RenderQueue *renderQueue)
{
    if (RenderResource<RHI::MaterialRenderData>::PrepareForRendering(renderQueue))
    {
        shader->PrepareForRendering(renderQueue);

        clientRenderData.renderMode = renderMode;

        for (auto it = this->FloatParamsBegin(), end = this->FloatParamsEnd(); it != end; ++it)
            clientRenderData.parameters.AddFloat(*it);
        for (auto it = this->VectorParamsBegin(), end = this->VectorParamsEnd(); it != end; ++it)
            clientRenderData.parameters.AddVector(*it);
        for (auto it = this->MatrixParamsBegin(), end = this->MatrixParamsEnd(); it != end; ++it)
            clientRenderData.parameters.AddMatrix(*it);
        for (auto it = this->TextureParamsBegin(), end = this->TextureParamsEnd(); it != end; ++it)
        {
            it->value->PrepareForRendering(renderQueue);

            clientRenderData.parameters.AddTexture(*it);
        }
    }

    return true;
}

} // namespace Framework
