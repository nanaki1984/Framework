#include "Components/Renderer.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Collections/Array.h"
#include "Core/Collections/List.h"
#include "Render/Resources/ResourceServer.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"

namespace Framework {

DefineClassInfo(Framework::Renderer, Framework::Component);
DefineComponent(Framework::Renderer, -1000000);

Renderer::Renderer()
: materials(Memory::GetAllocator<MallocAllocator>()),
  sortingOrder(0)
{ }

Renderer::Renderer(const Renderer &other)
: Component(other),
  bounds(other.bounds),
  mesh(other.mesh),
  materials(other.materials),
  sortingOrder(other.sortingOrder)
{ }

Renderer::Renderer(Renderer &&other)
: Component(std::forward<Component>(other)),
  bounds(other.bounds),
  mesh(std::forward<WeakPtr<Mesh>>(other.mesh)),
  materials(std::forward<Array<WeakPtr<Material>>>(other.materials)),
  sortingOrder(other.sortingOrder)
{ }

Renderer::~Renderer()
{ }

const Math::Bounds&
Renderer::GetBounds() const
{
    return bounds;
}

uint32_t
Renderer::GetMaterialsCount() const
{
    return materials.Count();
}

const WeakPtr<Material>&
Renderer::GetMaterial(uint32_t index) const
{
    return materials[index];
}

void
Renderer::SetMaterial(const WeakPtr<Material> &material, uint32_t index)
{
    materials[index] = material;
}

void
Renderer::SetMaterial(const char *filename, Resource::Access access, uint32_t index)
{
    materials[index] = ResourceServer::Instance()->NewResourceFromFile<Material>(filename, access);
}

uint8_t
Renderer::GetSortingOrder() const
{
    return sortingOrder;
}

void
Renderer::SetSortingOrder(uint8_t value)
{
    sortingOrder = value;
}

void
Renderer::OnCreate()
{
    Component::OnCreate();
    assert(!entity->renderer.IsValid() || entity->renderer.EqualsTo(this));
    entity->renderer = this;
}

} // namespace Framework
