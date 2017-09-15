#include "Render/MaterialParamsBlock.h"
#include "Core/Memory/MallocAllocator.h"

namespace Framework {

MaterialParamsBlock::MaterialParamsBlock()
: floatParams  (Memory::GetAllocator<MallocAllocator>()),
  vectorParams (Memory::GetAllocator<MallocAllocator>()),
  matrixParams (Memory::GetAllocator<MallocAllocator>()),
  textureParams(Memory::GetAllocator<MallocAllocator>()),
  bufferParams (Memory::GetAllocator<MallocAllocator>())
{ }

MaterialParamsBlock::MaterialParamsBlock(const MaterialParamsBlock &other)
: floatParams  (other.floatParams),
  vectorParams (other.vectorParams),
  matrixParams (other.matrixParams),
  textureParams(other.textureParams),
  bufferParams (other.bufferParams)
{ }

MaterialParamsBlock::MaterialParamsBlock(MaterialParamsBlock &&other)
: floatParams  (std::forward<Array<Material::FloatParam>>  (other.floatParams)),
  vectorParams (std::forward<Array<Material::VectorParam>> (other.vectorParams)),
  matrixParams (std::forward<Array<Material::MatrixParam>> (other.matrixParams)),
  textureParams(std::forward<Array<Material::TextureParam>>(other.textureParams)),
  bufferParams (std::forward<Array<Material::BufferParam>> (other.bufferParams))
{ }

MaterialParamsBlock::~MaterialParamsBlock()
{ }

MaterialParamsBlock&
MaterialParamsBlock::operator =(const MaterialParamsBlock &other)
{
	floatParams   = other.floatParams;
	vectorParams  = other.vectorParams;
	matrixParams  = other.matrixParams;
    textureParams = other.textureParams;
	bufferParams  = other.bufferParams;
	return (*this);
}

MaterialParamsBlock&
MaterialParamsBlock::operator =(MaterialParamsBlock &&other)
{
	floatParams   = std::forward<Array<Material::FloatParam>>  (other.floatParams);
	vectorParams  = std::forward<Array<Material::VectorParam>> (other.vectorParams);
	matrixParams  = std::forward<Array<Material::MatrixParam>> (other.matrixParams);
    textureParams = std::forward<Array<Material::TextureParam>>(other.textureParams);
    bufferParams  = std::forward<Array<Material::BufferParam>> (other.bufferParams);
	return (*this);
}

void
MaterialParamsBlock::Clear()
{
    floatParams.Clear();
    vectorParams.Clear();
    matrixParams.Clear();
    textureParams.Clear();
	bufferParams.Clear();
}

void
MaterialParamsBlock::AddFloat(const StringHash &name, float value)
{
    floatParams.PushBack({ name, value });
}

const Material::FloatParam*
MaterialParamsBlock::FloatParamsBegin() const
{
    return floatParams.Begin();
}

const Material::FloatParam*
MaterialParamsBlock::FloatParamsEnd() const
{
    return floatParams.End();
}

void
MaterialParamsBlock::AddVector(const StringHash &name, const Math::Vector4 &value)
{
    vectorParams.PushBack({ name, value });
}

const Material::VectorParam*
MaterialParamsBlock::VectorParamsBegin() const
{
    return vectorParams.Begin();
}

const Material::VectorParam*
MaterialParamsBlock::VectorParamsEnd() const
{
    return vectorParams.End();
}

void
MaterialParamsBlock::AddMatrix(const StringHash &name, const Math::Matrix &value)
{
    matrixParams.PushBack({ name, value });
}

const Material::MatrixParam*
MaterialParamsBlock::MatrixParamsBegin() const
{
    return matrixParams.Begin();
}

const Material::MatrixParam*
MaterialParamsBlock::MatrixParamsEnd() const
{
    return matrixParams.End();
}

void
MaterialParamsBlock::AddTexture(const StringHash &name, const WeakPtr<Texture> &value)
{
    textureParams.PushBack({ name, value });
}

const Material::TextureParam*
MaterialParamsBlock::TextureParamsBegin() const
{
    return textureParams.Begin();
}

const Material::TextureParam*
MaterialParamsBlock::TextureParamsEnd() const
{
    return textureParams.End();
}

void
MaterialParamsBlock::AddBuffer(const StringHash &name, const SmartPtr<RHI::ComputeBuffer> &value)
{
	bufferParams.PushBack({ name, value });
}

const Material::BufferParam*
MaterialParamsBlock::BufferParamsBegin() const
{
	return bufferParams.Begin();
}

const Material::BufferParam*
MaterialParamsBlock::BufferParamsEnd() const
{
	return bufferParams.End();
}

} // namespace Framework
