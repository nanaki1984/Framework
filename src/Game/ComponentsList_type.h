#pragma once

#include "Core/Debug.h"
#include "Game/BaseComponentsList.h"
#include "Core/Pool/Pool_type.h"

namespace Framework {

class Entity;

template <typename T>
class ComponentsList : public BaseComponentsList {
    DeclareClassInfo;
private:
    static ComponentsList *instance;

    static void Initialize();

    Pool<T> pool;
public:
    ComponentsList();
    virtual ~ComponentsList();

    virtual void OnUpdate();
    virtual void OnLateUpdate();
	virtual void OnRender();
    virtual void OnPause();
    virtual void OnResume();
    virtual void OnQuit();

    virtual Handle<Component> NewComponentInstance(Entity *entity);
    virtual Handle<Component> NewComponentToDeserialize(Entity *entity);
    virtual bool TryDeserializeComponent(Component *component, SerializationServer *server, const BitStream &stream);

    virtual const ClassInfo* GetComponentClassInfo() const;

    static ComponentsList* Instance();

    Handle<T> AttachToEntity(Entity *entity);

    template <typename... Args>
    Handle<T> AttachToEntity(Entity *entity, Args... arguments);

    T* Begin();
    const T* Begin() const;
    T* End();
    const T* End() const;

//#if defined(EDITOR)
    virtual void OnSerializeComponents(BitStream &stream);
//#endif
};

} // namespace Framework
