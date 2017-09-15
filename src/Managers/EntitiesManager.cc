#include "Managers/EntitiesManager.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/MallocAllocator.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"
#include "Components/Transform.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::EntitiesManager, Framework::BaseManager);
DefineManager(Framework::EntitiesManager, 0);

EntitiesManager::EntitiesManager()
: entities(Memory::GetAllocator<MallocAllocator>())
{ }

EntitiesManager::~EntitiesManager()
{ }

Handle<Entity>
EntitiesManager::CloneRecursively(Handle<Entity> source, Handle<Entity> cloneParent)
{
    Handle<Entity> newEntity = entities.CloneInstance(source);

    const TransformChildren &children = source->GetTransform()->GetChildren();

    auto child = children.GetFirst();
    while (child.IsValid())
    {
        this->CloneRecursively(child->GetEntity(), newEntity);

        child = children.GetNext(child);
    }

    if (cloneParent.IsValid())
        newEntity->GetTransform()->SetParent(cloneParent->GetTransform(), false);

    return newEntity;
}

Handle<Entity>
EntitiesManager::NewVoidEntity()
{
    return entities.NewInstance();
}

Handle<Entity>
EntitiesManager::NewEntity()
{
	Handle<Entity> newEntity = entities.NewInstance();
	newEntity->AddComponent<Transform>();
	return newEntity;
}

Handle<Entity>
EntitiesManager::NewEntity(const StringHash &name)
{
	Handle<Entity> newEntity = entities.NewInstance();
	newEntity->AddComponent<Transform>(name);
	return newEntity;
}

Handle<Entity>
EntitiesManager::NewEntity(const StringHash &name, const Handle<Entity> &parent)
{
	Handle<Entity> newEntity = entities.NewInstance();
	newEntity->AddComponent<Transform>(name, parent->GetTransform());
	return newEntity;
}

Handle<Entity>
EntitiesManager::CloneEntity(const Handle<Entity> &source, bool cloneRecursivelty)
{
    Handle<Entity> newEntity;
    if (cloneRecursivelty)
        newEntity = this->CloneRecursively(source, Handle<Entity>::Null);
    else
        newEntity = entities.CloneInstance(source);
	return newEntity;
}

Handle<Entity>
EntitiesManager::CloneEntity(const Handle<Entity> &source, const StringHash &cloneName, bool cloneRecursivelty)
{
    Handle<Entity> newEntity;
    if (cloneRecursivelty)
        newEntity = this->CloneRecursively(source, Handle<Entity>::Null);
    else
        newEntity = entities.CloneInstance(source);
	newEntity->GetTransform()->SetName(cloneName);
	return newEntity;
}

Handle<Entity>
EntitiesManager::CloneEntity(const Handle<Entity> &source, const StringHash &cloneName, const Handle<Entity> &cloneParent, bool cloneRecursivelty)
{
    Handle<Entity> newEntity;
    if (cloneRecursivelty)
        newEntity = this->CloneRecursively(source, cloneParent);
    else
    {
        newEntity = entities.CloneInstance(source);
        newEntity->GetTransform()->SetParent(cloneParent->GetTransform(), false);
    }
	newEntity->GetTransform()->SetName(cloneName);
	return newEntity;
}

void
EntitiesManager::OnUpdate()
{ }

void
EntitiesManager::OnLateUpdate()
{ }

void
EntitiesManager::OnRender()
{ }

void
EntitiesManager::OnPause()
{ }

void
EntitiesManager::OnResume()
{ }

void
EntitiesManager::OnQuit()
{
    entities.Clear();
}

} // namespace Framework
