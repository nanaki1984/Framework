#pragma once

#include "Core/RefCounted.h"
#include "Game/Component.h"

namespace Framework {

class ClassInfo;
class Entity;

class BaseComponentsList : public RefCounted {
    DeclareClassInfo;
protected:
    int executionOrder;

    void AttachComponentToEntity(Entity *entity, Component *component);
    void DetachComponentFromEntity(Entity *entity, Component *component);

	Component::State GetComponentState(Component *component);
	void SetComponentState(Component *component, Component::State newState);

//#if defined(EDITOR)
    uint32_t GetComponentSerializationId(Component *component);
//#endif
public:
    BaseComponentsList(int _executionOrder);
    virtual ~BaseComponentsList();

    virtual void OnUpdate() = 0;
    virtual void OnLateUpdate() = 0;
	virtual void OnRender() = 0;
    virtual void OnPause() = 0;
    virtual void OnResume() = 0;
    virtual void OnQuit() = 0;

    virtual Handle<Component> NewComponentInstance(Entity *entity) = 0;
    virtual Handle<Component> NewComponentToDeserialize(Entity *entity) = 0;
    virtual bool TryDeserializeComponent(Component *component, SerializationServer *server, const BitStream &stream) = 0;

    virtual const ClassInfo* GetComponentClassInfo() const = 0;

    int Compare(const BaseComponentsList &other) const;

//#if defined(EDITOR)
    virtual void OnSerializeComponents(BitStream &stream) = 0;
//#endif

    friend class Component;
};

} // namespace Framework
