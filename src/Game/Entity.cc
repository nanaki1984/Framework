#include "Game/Entity.h"
#include "Game/ComponentsList.h"
#include "Components/Renderer.h"
#include "Core/Memory/ScratchAllocator.h"

namespace Framework {

DefineClassInfo(Framework::Entity, Framework::BaseObject);

Entity::Entity()
: activeInHierarchy(true),
  branchActive(true)
//#if defined(EDITOR)
, serializationId(kInvalidSerializationId)
//#endif
{ }

Entity::Entity(const Entity &other)
: activeInHierarchy(other.activeInHierarchy),
  branchActive(other.branchActive)
{
    Handle<Component> comp = other.components.HandleBegin();
    while (comp.IsValid()) {
        comp->CloneTo(this);
		comp = other.components.GetNext(comp);
    }
}

Entity::Entity(Entity &&other)
: BaseObject(std::forward<BaseObject>(other)),
  components(std::forward<HandleList<Component, &Component::entityNode>>(other.components)),
  activeInHierarchy(other.activeInHierarchy),
  branchActive(other.branchActive),
  transform(other.transform),
  renderer(other.renderer)
{ }

Entity::~Entity()
{
    Component *comp = components.Begin();
    while (comp != nullptr) {
        comp->Destroy();
        comp = components.Begin();
    }
}

bool
Entity::IsActiveInHierarchy() const
{
	return activeInHierarchy;
}

bool
Entity::IsActive() const
{
	return branchActive && activeInHierarchy;
}

void
Entity::SetActiveRecursively(bool flag)
{
	branchActive = flag;

	auto it = components.Begin();
	while (it != nullptr) {
		it->entityActive = this->IsActive();

		it = components.GetNext(it);
	}

	auto child = transform->GetChildren().GetFirst();
	while (child.IsValid()) {
		child->GetEntity()->SetActiveRecursively(flag);

		child = transform->GetChildren().GetNext(child);
	}
}

void
Entity::SetActive(bool flag)
{
	if (flag == this->IsActive())
		return;

	activeInHierarchy = flag;

	this->SetActiveRecursively(flag);
}

const Handle<Transform>&
Entity::GetTransform() const
{
    if (!transform.IsValid())
        transform = this->GetComponent<Transform>();
    return transform;
}

const Handle<Renderer>&
Entity::GetRenderer() const
{
    if (!renderer.IsValid())
        renderer = this->GetComponent<Renderer>();
    return renderer;
}

Handle<Component>
Entity::AddComponent(const StringHash &className)
{
    return GetManager<ComponentsManager>()->GetComponentsList(className)->NewComponentInstance(this);
}

void
Entity::DestroyRecursively()
{
    Array<Handle<Transform>> fullHierarchy(Memory::GetAllocator<ScratchAllocator>());

    this->GetComponentsRecursively<Transform>(fullHierarchy);

    for (int32_t i = fullHierarchy.Count() - 1; i >= 0; --i)
        fullHierarchy[i]->GetEntity()->Destroy();
}

//#if defined(EDITOR)
uint32_t
Entity::GetSerializationId() const
{
    return serializationId;
}

void
Entity::MarkForSerialization(uint32_t &nextUniqueId)
{
    serializationId = nextUniqueId++;

    auto it = components.Begin();
    while (it != nullptr) {
        it->MarkForSerialization(nextUniqueId);

        it = components.GetNext(it);
    }
}

void
Entity::ClearSerializationIds()
{
    serializationId = kInvalidSerializationId;

    auto it = components.Begin();
    while (it != nullptr) {
        it->ClearSerializationId();

        it = components.GetNext(it);
    }
}

void
Entity::Serialize(BitStream &stream)
{
    stream << serializationId
           << activeInHierarchy
           << branchActive
           << components.Count();

    auto it = components.Begin();
    while (it != nullptr) {
        stream << it->GetSerializationId()
               << String(Memory::GetAllocator<ScratchAllocator>(), it->GetTypeName());

        it = components.GetNext(it);
    }
}
//#endif

void
Entity::Deserialize(SerializationServer *server, const BitStream &stream)
{
    uint32_t serializationId;

    stream >> serializationId
           >> activeInHierarchy
           >> branchActive;

    server->AddLoadingObject(serializationId, this);

    uint32_t componentsCount;
    stream >> componentsCount;

    auto compMng = GetManager<ComponentsManager>();
    for (uint32_t i = 0; i < componentsCount; ++i)
    {
        String typeName(Memory::GetAllocator<ScratchAllocator>());

        stream >> serializationId
               >> typeName;

        auto component = compMng->GetComponentsList(typeName)->NewComponentToDeserialize(this);
        component->entityActive = this->IsActive();

        server->AddLoadingObject(serializationId, component);
    }
}

} // namespace Framework
