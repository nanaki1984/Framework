#include "Game/BaseComponentsList.h"
#include "Game/Entity.h"

namespace Framework {

DefineAbstractClassInfo(Framework::BaseComponentsList, Framework::RefCounted);

BaseComponentsList::BaseComponentsList(int _executionOrder)
: executionOrder(_executionOrder)
{ }

BaseComponentsList::~BaseComponentsList()
{ }

void
BaseComponentsList::AttachComponentToEntity(Entity *entity, Component *component)
{
    assert(nullptr == component->entity);
    entity->components.PushBack(component);
    component->entity = entity;
}

void
BaseComponentsList::DetachComponentFromEntity(Entity *entity, Component *component)
{
    assert(entity == component->entity);
    entity->components.Remove(component);
    component->entity = nullptr;
}

Component::State
BaseComponentsList::GetComponentState(Component *component)
{
	return component->state;
}

void
BaseComponentsList::SetComponentState(Component *component, Component::State newState)
{
	component->state = newState;
}

int
BaseComponentsList::Compare(const BaseComponentsList &other) const
{
    if (executionOrder > other.executionOrder)
        return 1;
    else if (executionOrder < other.executionOrder)
        return -1;
    else
        return 0;
}

//#if defined(EDITOR)
uint32_t
BaseComponentsList::GetComponentSerializationId(Component *component)
{
    return component->GetSerializationId();
}
//#endif

} // namespace Framework
