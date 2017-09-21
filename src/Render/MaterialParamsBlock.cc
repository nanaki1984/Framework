#include "Render/MaterialParamsBlock.h"
#include "Core/Collections/Array.h"
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
: floatParams  (std::forward<Array<Materials::FloatParam>>  (other.floatParams)),
  vectorParams (std::forward<Array<Materials::VectorParam>> (other.vectorParams)),
  matrixParams (std::forward<Array<Materials::MatrixParam>> (other.matrixParams)),
  textureParams(std::forward<Array<Materials::TextureParam>>(other.textureParams)),
  bufferParams (std::forward<Array<Materials::BufferParam>> (other.bufferParams))
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
	floatParams   = std::forward<Array<Materials::FloatParam>>  (other.floatParams);
	vectorParams  = std::forward<Array<Materials::VectorParam>> (other.vectorParams);
	matrixParams  = std::forward<Array<Materials::MatrixParam>> (other.matrixParams);
    textureParams = std::forward<Array<Materials::TextureParam>>(other.textureParams);
    bufferParams  = std::forward<Array<Materials::BufferParam>> (other.bufferParams);
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

void
MaterialParamsBlock::AddFloat(const Materials::FloatParam &param)
{
    floatParams.PushBack(param);
}

const Materials::FloatParam*
MaterialParamsBlock::FloatParamsBegin() const
{
    return floatParams.Begin();
}

const Materials::FloatParam*
MaterialParamsBlock::FloatParamsEnd() const
{
    return floatParams.End();
}

void
MaterialParamsBlock::AddVector(const StringHash &name, const Math::Vector4 &value)
{
    vectorParams.PushBack({ name, value });
}

void
MaterialParamsBlock::AddVector(const Materials::VectorParam &param)
{
    vectorParams.PushBack(param);
}

const Materials::VectorParam*
MaterialParamsBlock::VectorParamsBegin() const
{
    return vectorParams.Begin();
}

const Materials::VectorParam*
MaterialParamsBlock::VectorParamsEnd() const
{
    return vectorParams.End();
}

void
MaterialParamsBlock::AddMatrix(const StringHash &name, const Math::Matrix &value)
{
    matrixParams.PushBack({ name, value });
}

void
MaterialParamsBlock::AddMatrix(const Materials::MatrixParam &param)
{
    matrixParams.PushBack(param);
}

const Materials::MatrixParam*
MaterialParamsBlock::MatrixParamsBegin() const
{
    return matrixParams.Begin();
}

const Materials::MatrixParam*
MaterialParamsBlock::MatrixParamsEnd() const
{
    return matrixParams.End();
}

void
MaterialParamsBlock::AddTexture(const StringHash &name, const WeakPtr<Texture> &value)
{
    textureParams.PushBack({ name, value });
}

void
MaterialParamsBlock::AddTexture(const Materials::TextureParam &param)
{
    textureParams.PushBack(param);
}

const Materials::TextureParam*
MaterialParamsBlock::TextureParamsBegin() const
{
    return textureParams.Begin();
}

const Materials::TextureParam*
MaterialParamsBlock::TextureParamsEnd() const
{
    return textureParams.End();
}

void
MaterialParamsBlock::AddBuffer(const StringHash &name, const SmartPtr<RHI::ComputeBuffer> &value)
{
	bufferParams.PushBack({ name, value });
}

void
MaterialParamsBlock::AddBuffer(const Materials::BufferParam &param)
{
    bufferParams.PushBack(param);
}

const Materials::BufferParam*
MaterialParamsBlock::BufferParamsBegin() const
{
	return bufferParams.Begin();
}

const Materials::BufferParam*
MaterialParamsBlock::BufferParamsEnd() const
{
	return bufferParams.End();
}

} // namespace Framework
