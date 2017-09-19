#pragma once

#include "Core/SmartPtr.h"
#include "Render/Resources/Resource.h"
#include "Render/RenderObjects.h"
#include "Core/Collections/Array_type.h"
#include "Core/IO/BitStream.h"
#include "Math/Bounds.h"

namespace Framework {

class Mesh : public Resource {
    DeclareClassInfo;
protected:
    SmartPtr<RHI::VertexBuffer> vertexBuffer;
    SmartPtr<RHI::IndexBuffer> indexBuffer;

    BitStream vertexBufferData;
    BitStream indexBufferData;

    Array<Math::Bounds> subMeshesBounds;
    Array<DrawPrimitives> subMeshesPrimitives;

    virtual bool LoadImpl();
    virtual void UnloadImpl();
    virtual bool CloneImpl(WeakPtr<Resource> source);
    virtual uint32_t ComputeSize();
public:
    Mesh();
    virtual ~Mesh();

    virtual uint32_t GetAccessModes() const;

    void Load(RHI::VertexBufferDesc &&vbDesc, const RHI::IndexBufferDesc &ibDesc, uint32_t subMeshCount = 1);
    void Load(const SmartPtr<RHI::VertexBuffer> &vb, const SmartPtr<RHI::IndexBuffer> &ib, uint32_t subMeshCount = 1);

    void ApplyVertices(uint32_t offset = 0, uint32_t size = 0);
    void ApplyIndices(uint32_t offset = 0, uint32_t size = 0);
    void Apply();

    void RefreshBounds();

    const RHI::VertexDecl& GetVertexDecl() const;

    const SmartPtr<RHI::VertexBuffer>& GetVertexBuffer() const;
    const SmartPtr<RHI::IndexBuffer>& GetIndexBuffer() const;

    BitStream& GetVertexBufferData(); // TEMP
    BitStream& GetIndexBufferData(); // TEMP

    uint32_t GetSubMeshCount() const;
    void SetSubMeshCount(int subMeshCount);

    const Math::Bounds& GetSubMeshBounds(uint32_t index) const;
    Math::Bounds& GetSubMeshBounds(uint32_t index);

    const DrawPrimitives& GetSubMeshPrimitives(uint32_t index) const;
    DrawPrimitives& GetSubMeshPrimitives(uint32_t index);

    // ToDo: vertices & triangles iterators
};

inline const RHI::VertexDecl&
Mesh::GetVertexDecl() const
{
    return vertexBuffer->GetVertexDecl();
}

inline const SmartPtr<RHI::VertexBuffer>&
Mesh::GetVertexBuffer() const
{
    return vertexBuffer;
}

inline const SmartPtr<RHI::IndexBuffer>&
Mesh::GetIndexBuffer() const
{
    return indexBuffer;
}

inline BitStream&
Mesh::GetVertexBufferData()
{
    return vertexBufferData;
}

inline BitStream&
Mesh::GetIndexBufferData()
{
    return indexBufferData;
}

inline uint32_t
Mesh::GetSubMeshCount() const
{
    assert(isLoaded);
    return subMeshesBounds.Count();
}

inline const Math::Bounds&
Mesh::GetSubMeshBounds(uint32_t index) const
{
    assert(isLoaded);
    return subMeshesBounds[index];
}

inline Math::Bounds&
Mesh::GetSubMeshBounds(uint32_t index)
{
    assert(isLoaded);
    return subMeshesBounds[index];
}

inline const DrawPrimitives&
Mesh::GetSubMeshPrimitives(uint32_t index) const
{
    assert(isLoaded);
    return subMeshesPrimitives[index];
}

inline DrawPrimitives&
Mesh::GetSubMeshPrimitives(uint32_t index)
{
    assert(isLoaded);
    return subMeshesPrimitives[index];
}

} // namespace Framework
