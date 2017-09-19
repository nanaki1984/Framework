#include "Render/Resources/Mesh.h"
#include "Core/Collections/Array.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/BlocksAllocator.h"
#include "Core/IO/FileServer.h"
#include "Core/Log.h"
#include "Render/RenderQueue.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::Mesh, Framework::Resource);

Mesh::Mesh()
: vertexBufferData(Memory::GetAllocator<MallocAllocator>()),
  indexBufferData(Memory::GetAllocator<MallocAllocator>()),
  subMeshesBounds(Memory::GetAllocator<MallocAllocator>()),
  subMeshesPrimitives(Memory::GetAllocator<MallocAllocator>())
{ }

Mesh::~Mesh()
{ }

uint32_t
Mesh::GetAccessModes() const
{
    return ReadOnly | Writable;
}

bool
Mesh::LoadImpl()
{
    BitStream stream(Memory::GetAllocator<ScratchAllocator>());
    if (FileServer::Instance()->ReadOnly(filename.AsCString(), stream) != 0)
        return false;

    char magic[4];
    stream.ReadBytes(magic, 3);
    magic[3] = '\0';

    bool isMesh = (0 == strcmp(magic, "MSH")), isCompressed = (0 == strcmp(magic, "MSC"));
    isMesh |= isCompressed;

    if (!isMesh)
        return false;

    RHI::VertexBufferDesc vbDesc;
    vbDesc.flags = RHI::HardwareBuffer::NoFlags;

    uint8_t i = 0, nElements;
    stream >> nElements;

    for (; i < nElements; ++i) {
        uint8_t elemType, elemSize;

        stream >> elemType;
        stream >> elemSize;

        vbDesc.vertexDecl.AddVertexElement(static_cast<RHI::VertexDecl::VertexType>(elemType),
                                           static_cast<RHI::VertexDecl::VertexSize>(elemSize));
    }

    uint8_t subMeshCount;
    stream >> subMeshCount;
    subMeshesBounds.Resize(subMeshCount);
    subMeshesPrimitives.Resize(subMeshCount);

    for (i = 0; i < subMeshCount; ++i) {
        uint32_t tmp;

        DrawPrimitives &tris = subMeshesPrimitives[i];
        tris.primType = DrawPrimitives::TriangleList;
        stream >> tmp;
        tris.startIndex = tmp;
        stream >> tmp;
        tris.nPrimitives = tmp;
        //tris.endIndex = tris.startIndex + tmp * 3 - 1;
        //tris.nVertices = tris.nPrimitives * 3;

        Math::Bounds &bounds = subMeshesBounds[i];
        stream >> bounds.min.x;
        stream >> bounds.min.y;
        stream >> bounds.min.z;
        stream >> bounds.max.x;
        stream >> bounds.max.y;
        stream >> bounds.max.z;
    }

    RHI::IndexBufferDesc ibDesc;
    ibDesc.flags = RHI::HardwareBuffer::NoFlags;
    ibDesc.indexSize = RHI::IndexWord;
    stream >> ibDesc.nIndices;

    indexBufferData.Reserve(ibDesc.indexSize * ibDesc.nIndices);
    indexBufferData.WriteBytes(stream.GetReadPos(), ibDesc.indexSize * ibDesc.nIndices);
    indexBufferData.Reset();
    stream.SkipBytes(ibDesc.indexSize * ibDesc.nIndices);

    RHI::LockInfo lockInfo;
    Memory::Zero(&lockInfo);

    indexBuffer = SmartPtr<RHI::IndexBuffer>::MakeNew<BlocksAllocator>(ibDesc);
    indexBuffer->Upload(lockInfo, indexBufferData.GetData());

    if (isCompressed) {
        Math::Bounds meshBounds;
        meshBounds.Reset();
        for (i = 0; i < subMeshCount; ++i)
            meshBounds.Encapsulate(subMeshesBounds[i]);

        uint32_t vtxStride = 0;
        for (i = 0; i < nElements; ++i) {
            const RHI::VertexDecl::VertexElement &elem = vbDesc.vertexDecl.GetElement(i);
            switch (elem.type) {
                case RHI::VertexDecl::XYZ:
                case RHI::VertexDecl::Normal:
                case RHI::VertexDecl::Tangent:
                case RHI::VertexDecl::Binormal:
                case RHI::VertexDecl::TexCoord:
                    vtxStride += (elem.size >> 1);
                    break;
                default:
                    vtxStride += elem.size;
                    break;
            }
        }

        uint32_t vtxIndex = 0,
                 vtxCount = vbDesc.nVertices = stream.RemainingBytes() / vtxStride;
        float qMin[4], qMax[4];
        uint16_t qValue;
        uint32_t fIndex, fCount;
        vertexBufferData.Reserve(vbDesc.nVertices * vbDesc.vertexDecl.GetVertexStride());
        for (; vtxIndex < vtxCount; ++vtxIndex) {
            for (i = 0; i < nElements; ++i) {
                const RHI::VertexDecl::VertexElement &elem = vbDesc.vertexDecl.GetElement(i);
                switch (elem.type) {
                    case RHI::VertexDecl::XYZ:
                    case RHI::VertexDecl::Normal:
                    case RHI::VertexDecl::Tangent:
                    case RHI::VertexDecl::Binormal:
                    case RHI::VertexDecl::TexCoord:
                        if (RHI::VertexDecl::TexCoord == elem.type) {
                            qMin[0] = qMin[1] = qMin[2] = qMin[3] = -16.0f;
                            qMax[0] = qMax[1] = qMax[2] = qMax[3] =  16.0f;
                        } else if (RHI::VertexDecl::XYZ == elem.type) {
                            qMin[0] = meshBounds.min.x;
                            qMin[1] = meshBounds.min.y;
                            qMin[2] = meshBounds.min.z;
                            qMax[0] = meshBounds.max.x;
                            qMax[1] = meshBounds.max.y;
                            qMax[2] = meshBounds.max.z;
                        } else { // Normal, Tangent, Binormal
                            qMin[0] = qMin[1] = qMin[2] = qMin[3] = -1.0f;
                            qMax[0] = qMax[1] = qMax[2] = qMax[3] =  1.0f;
                        }

                        fCount = (elem.size >> 2);
                        for (fIndex = 0; fIndex < fCount; ++fIndex) {
                            stream >> qValue;
                            vertexBufferData << ((qMin[fIndex] + (qMax[fIndex] - qMin[fIndex]) * (qValue / 65535.0f)));
                        }
                        break;
                    default:
                        vertexBufferData.WriteBytes(stream.GetReadPos(), elem.size);
                        stream.SkipBytes(elem.size);
                        break;
                }
            }
        }
        vertexBufferData.Reset();
    } else {
        uint32_t vtxStride = vbDesc.vertexDecl.GetVertexStride();
        vbDesc.nVertices = stream.RemainingBytes() / vtxStride;

        vertexBufferData.Reserve(vbDesc.nVertices * vtxStride);
        vertexBufferData.WriteBytes(stream.GetReadPos(), vbDesc.nVertices * vtxStride);
        vertexBufferData.Reset();
    }

    vertexBuffer = SmartPtr<RHI::VertexBuffer>::MakeNew<BlocksAllocator>(std::move(vbDesc));
    vertexBuffer->Upload(lockInfo, vertexBufferData.GetData());

    RenderQueue::Instance()->GetRenderer()->OnMeshLoaded(this);

    return true;
}

void
Mesh::UnloadImpl()
{
    RenderQueue::Instance()->GetRenderer()->OnMeshUnloaded(this);

    vertexBuffer.Reset();
    indexBuffer.Reset();

    vertexBufferData = BitStream(Memory::GetAllocator<MallocAllocator>());
    indexBufferData = BitStream(Memory::GetAllocator<MallocAllocator>());

    subMeshesBounds.SetCapacity(0);
    subMeshesPrimitives.SetCapacity(0);
}

bool
Mesh::CloneImpl(WeakPtr<Resource> source)
{
    Mesh *mesh = source.Cast<Mesh>();

    RHI::VertexBufferDesc vbDesc;
    vbDesc.flags      = mesh->vertexBuffer->GetFlags();
    vbDesc.vertexDecl = mesh->vertexBuffer->GetVertexDecl();
    vbDesc.nVertices  = mesh->vertexBuffer->GetSize() / vbDesc.vertexDecl.GetVertexStride();

    vertexBuffer = SmartPtr<RHI::VertexBuffer>::MakeNew<BlocksAllocator>(std::move(vbDesc));

    RHI::IndexBufferDesc ibDesc;
    ibDesc.flags     = mesh->indexBuffer->GetFlags();
    ibDesc.indexSize = mesh->indexBuffer->GetIndexSize();
    ibDesc.nIndices  = mesh->indexBuffer->GetSize() / ibDesc.indexSize;

    indexBuffer = SmartPtr<RHI::IndexBuffer>::MakeNew<BlocksAllocator>(ibDesc);

    vertexBufferData = mesh->vertexBufferData;
    vertexBufferData.Reset();

    indexBufferData = mesh->indexBufferData;
    indexBufferData.Reset();

    subMeshesBounds = mesh->subMeshesBounds;
    subMeshesPrimitives = mesh->subMeshesPrimitives;

    RHI::LockInfo lockInfo;
    Memory::Zero(&lockInfo);
    vertexBuffer->Upload(lockInfo, vertexBufferData.GetData());
    indexBuffer->Upload(lockInfo, indexBufferData.GetData());

    return true;
}

uint32_t
Mesh::ComputeSize()
{
    return vertexBuffer->GetSize() + indexBuffer->GetSize();
}

void
Mesh::Load(RHI::VertexBufferDesc &&vbDesc, const RHI::IndexBufferDesc &ibDesc, uint32_t subMeshCount)
{
    if (isLoaded)
        this->Unload();

    vertexBuffer = SmartPtr<RHI::VertexBuffer>::MakeNew<BlocksAllocator>(std::forward<RHI::VertexBufferDesc>(vbDesc));
    indexBuffer = SmartPtr<RHI::IndexBuffer>::MakeNew<BlocksAllocator>(ibDesc);

    vertexBufferData.Reserve(vertexBuffer->GetSize());
    indexBufferData.Reserve(indexBuffer->GetSize());

    assert(subMeshCount > 0);
    subMeshesBounds.Resize(subMeshCount);
    subMeshesPrimitives.Resize(subMeshCount);

    isLoaded = true;
}

void
Mesh::Load(const SmartPtr<RHI::VertexBuffer> &vb, const SmartPtr<RHI::IndexBuffer> &ib, uint32_t subMeshCount)
{
    if (isLoaded)
        this->Unload();

    vertexBuffer = vb;
    indexBuffer = ib;

    vertexBufferData.Reserve(vertexBuffer->GetSize());
    indexBufferData.Reserve(indexBuffer->GetSize());

    assert(subMeshCount > 0);
    subMeshesBounds.SetCapacity(subMeshCount);
    subMeshesPrimitives.SetCapacity(subMeshCount);

    isLoaded = true;
}

void
Mesh::ApplyVertices(uint32_t offset, uint32_t size)
{
    RHI::LockInfo lockInfo;
    Memory::Zero(&lockInfo);
    lockInfo.offset = offset;
    lockInfo.size = size;
    vertexBuffer->Upload(lockInfo, static_cast<const uint8_t*>(vertexBufferData.GetData()) + offset);
}

void
Mesh::ApplyIndices(uint32_t offset, uint32_t size)
{
    RHI::LockInfo lockInfo;
    Memory::Zero(&lockInfo);
    lockInfo.offset = offset;
    lockInfo.size = size;
    indexBuffer->Upload(lockInfo, static_cast<const uint8_t*>(indexBufferData.GetData()) + offset);
}

void
Mesh::Apply()
{
    RHI::LockInfo lockInfo;
    Memory::Zero(&lockInfo);
    vertexBuffer->Upload(lockInfo, vertexBufferData.GetData());
    indexBuffer->Upload(lockInfo, indexBufferData.GetData());
}

void
Mesh::RefreshBounds()
{
    // ToDo
}

void
Mesh::SetSubMeshCount(int subMeshCount)
{
    assert(subMeshCount > 0);
    subMeshesBounds.Resize(subMeshCount);
    subMeshesPrimitives.Resize(subMeshCount);
}

} // namespace Framework
