#include "Game/Component.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"
#include "Core/Pool/Handle.h"
#include "Core/Collections/Hash.h"
#include "Core/IO/BitStream.h"

namespace Framework {

DefineClassInfo(Framework::Component, Framework::BaseObject);
DefineComponent(Framework::Component, 0);

Component::Component()
: state(Creating),
  activeInEntity(true),
  entityActive(true)
//#if defined(EDITOR)
, serializationId(kInvalidSerializationId)
//#endif
{ }

Component::Component(const Component &other)
: BaseObject(other),
  state(Creating),
  activeInEntity(other.activeInEntity),
  entityActive(true)
{ }

Component::Component(Component &&other)
: BaseObject(std::forward<BaseObject>(other)),
  entity(other.entity),
  state(other.state),
  activeInEntity(other.activeInEntity),
  entityActive(other.entityActive),
  entityNode(other.entityNode)
{ }

Component::~Component()
{
    assert(entity.IsValid());
    this->GetBaseList()->DetachComponentFromEntity(entity, this);
    activeInEntity = false;
}

const Handle<Entity>&
Component::GetEntity() const
{
    return entity;
}

bool
Component::IsActive() const
{
	return entityActive && activeInEntity;
}

bool
Component::IsActiveInEntity() const
{
	return activeInEntity;
}

void
Component::SetActiveInEntity(bool flag)
{
	activeInEntity = flag;
}

Component*
Component::CloneTo(Entity *otherEntity) const
{
    auto myList = this->GetBaseList(); // this pointer could become invalid, because of pool growing, take it before
    Component *clone = static_cast<Component*>(this->GetPool()->Clone(this->GetInstanceID()));
    myList->AttachComponentToEntity(otherEntity, clone);
    return clone;
}

void
Component::Deserialize(SerializationServer *server, const BitStream &stream)
{
    stream >> activeInEntity;
}

void
Component::OnCreate()
{ }

void
Component::OnStart()
{ }

void
Component::Update()
{ }

void
Component::LateUpdate()
{ }

void
Component::Render()
{ }

void
Component::OnEnable()
{ }

void
Component::OnDisable()
{ }

void
Component::OnApplicationPause()
{ }

void
Component::OnApplicationResume()
{ }

void
Component::OnApplicationQuit()
{ }

//#if defined(EDITOR)
uint32_t
Component::GetSerializationId() const
{
    return serializationId;
}

void
Component::MarkForSerialization(uint32_t &nextUniqueId)
{
    serializationId = nextUniqueId++;
}

void
Component::ClearSerializationId()
{
    serializationId = kInvalidSerializationId;
}

void
Component::OnSerialize(BitStream &stream)
{
    stream << serializationId << activeInEntity;
}
//#endif

} // namespace Framework
