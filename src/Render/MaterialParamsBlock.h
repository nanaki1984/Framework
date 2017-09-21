#pragma once

#include "Core/StringHash.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Core/SmartPtr.h"
#include "Core/WeakPtr.h"
#include "Render/Resources/Texture.h"
#include "Render/RenderObjects.h"

namespace Framework {
    namespace Materials {

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

    } // namespace Materials

class MaterialParamsBlock {
private:
    Array<Materials::FloatParam>   floatParams;
    Array<Materials::VectorParam>  vectorParams;
    Array<Materials::MatrixParam>  matrixParams;
    Array<Materials::TextureParam> textureParams;
	Array<Materials::BufferParam>  bufferParams;
public:
    MaterialParamsBlock();
    MaterialParamsBlock(const MaterialParamsBlock &other);
    MaterialParamsBlock(MaterialParamsBlock &&other);
    ~MaterialParamsBlock();

    MaterialParamsBlock& operator =(const MaterialParamsBlock &other);
    MaterialParamsBlock& operator =(MaterialParamsBlock &&other);

    void Clear();

    void AddFloat(const StringHash &name, float value);
    void AddFloat(const Materials::FloatParam &param);
    const Materials::FloatParam* FloatParamsBegin() const;
    const Materials::FloatParam* FloatParamsEnd() const;

    void AddVector(const StringHash &name, const Math::Vector4 &value);
    void AddVector(const Materials::VectorParam &param);
    const Materials::VectorParam* VectorParamsBegin() const;
    const Materials::VectorParam* VectorParamsEnd() const;

    void AddMatrix(const StringHash &name, const Math::Matrix &value);
    void AddMatrix(const Materials::MatrixParam &value);
    const Materials::MatrixParam* MatrixParamsBegin() const;
    const Materials::MatrixParam* MatrixParamsEnd() const;

    void AddTexture(const StringHash &name, const WeakPtr<Texture> &value);
    void AddTexture(const Materials::TextureParam &value);
    const Materials::TextureParam* TextureParamsBegin() const;
    const Materials::TextureParam* TextureParamsEnd() const;
    
    void AddBuffer(const StringHash &name, const SmartPtr<RHI::ComputeBuffer> &value);
    void AddBuffer(const Materials::BufferParam &value);
    const Materials::BufferParam* BufferParamsBegin() const;
	const Materials::BufferParam* BufferParamsEnd() const;
};

} // namespace Framework
