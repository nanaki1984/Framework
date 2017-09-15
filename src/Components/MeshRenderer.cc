#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Render/Resources/ResourceServer.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"
#include "Core/Memory/ScratchAllocator.h"

namespace Framework {

DefineClassInfo(Framework::MeshRenderer, Framework::Renderer);
DefineComponent(Framework::MeshRenderer, -1000000);

MeshRenderer::MeshRenderer()
{ }

MeshRenderer::MeshRenderer(const MeshRenderer &other)
: Renderer(other)
{ }

MeshRenderer::MeshRenderer(MeshRenderer &&other)
: Renderer(std::forward<Renderer>(other))
{ }

MeshRenderer::~MeshRenderer()
{ }

void
MeshRenderer::UpdateBounds()
{
    if (!mesh.IsValid())
        return;

    Math::Bounds localBounds = mesh->GetSubMeshBounds(0);
    for (uint32_t i = 1, c = mesh->GetSubMeshCount(); i < c; ++i)
        localBounds.Encapsulate(mesh->GetSubMeshBounds(i));

    bounds = localBounds;
    bounds.Transform(entity->GetTransform()->GetLocalToWorld());
}

const WeakPtr<Mesh>&
MeshRenderer::GetMesh() const
{
    return mesh;
}

void
MeshRenderer::SetMesh(const WeakPtr<Mesh> &newMesh)
{
    assert(newMesh->IsLoaded());
    mesh = newMesh;
    this->UpdateBounds();
    materials.SetCapacity(mesh->GetSubMeshCount());
    materials.Resize(mesh->GetSubMeshCount());
}

void
MeshRenderer::SetMesh(const char *filename, Resource::Access access)
{
    mesh = ResourceServer::Instance()->NewResourceFromFile<Mesh>(filename, access);
    this->UpdateBounds();
    materials.SetCapacity(mesh->GetSubMeshCount());
    materials.Resize(mesh->GetSubMeshCount());
}

void
MeshRenderer::Deserialize(SerializationServer *server, const BitStream &stream)
{
    Component::Deserialize(server, stream);

    String str(Memory::GetAllocator<ScratchAllocator>());

    stream >> str;

    if (str.Length() > 0)
        this->SetMesh(str.AsCString());

    uint32_t materialsCount, realMatCount = materials.Count();
    stream >> materialsCount;
    for (uint32_t i = 0; i < materialsCount; ++i) {
        stream >> str;
        if (i < realMatCount && str.Length() > 0)
            this->SetMaterial(str.AsCString(), Resource::Access::ReadOnly, i);
    }

    stream >> sortingOrder;
}

void
MeshRenderer::Render()
{
    if (entity->GetTransform()->HasChanged())
        this->UpdateBounds();
}

//#if defined(EDITOR)
void
MeshRenderer::OnSerialize(BitStream &stream)
{
    Component::OnSerialize(stream);

    if (mesh.IsValid())
        stream << mesh->GetFilename();
    else
        stream << String::Empty;

    stream << materials.Count();
    for (auto it = materials.Begin(), end = materials.End(); it != end; ++it) {
        if (it->IsValid())
            stream << (*it)->GetFilename();
        else
            stream << String::Empty;
    }

    stream << sortingOrder;
}
//#endif

} // namespace Framework
