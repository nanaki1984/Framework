#pragma once

#include "Game/ComponentsList_type.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/BlocksAllocator.h"
#include "Core/SmartPtr.h"
#include "Managers/ComponentsManager.h"
#include "Managers/GetManager.h"

namespace Framework {

DefineTemplateClassInfo(Framework::ComponentsList, Framework::BaseComponentsList);

template <typename T> ComponentsList<T>* ComponentsList<T>::instance = nullptr;

template <typename T>
inline ComponentsList<T>*
ComponentsList<T>::Instance()
{
    if (nullptr == instance) // ToDo: mutex?
        Initialize();
    return instance;
}

template <typename T>
inline void
ComponentsList<T>::Initialize()
{
    assert(instance == nullptr);
    SmartPtr<BaseComponentsList> p = SmartPtr<BaseComponentsList>::MakeNew<BlocksAllocator, ComponentsList<T>>();
    GetManager<ComponentsManager>()->AddComponentsList(p);
}

template <typename T>
inline Handle<T>
ComponentsList<T>::AttachToEntity(Entity *entity)
{
    Handle<T> p = pool.NewInstance();
    this->AttachComponentToEntity(entity, p);

    p->OnCreate();
	this->SetComponentState(p, Component::Starting);

	return p;
}

template <typename T>
template <typename... Args>
inline Handle<T>
ComponentsList<T>::AttachToEntity(Entity *entity, Args... arguments)
{
	Handle<T> p = pool.NewInstance(arguments...);
    this->AttachComponentToEntity(entity, p);

    p->OnCreate();
	this->SetComponentState(p, Component::Starting);

	return p;
}

template <typename T>
inline T*
ComponentsList<T>::Begin()
{
    return pool.Begin();
}

template <typename T>
inline const T*
ComponentsList<T>::Begin() const
{
    return pool.Begin();
}

template <typename T>
inline T*
ComponentsList<T>::End()
{
    return pool.End();
}

template <typename T>
inline const T*
ComponentsList<T>::End() const
{
    return pool.End();
}

template <typename T>
inline
ComponentsList<T>::ComponentsList()
: BaseComponentsList(T::ExecutionOrder),
  pool(Memory::GetAllocator<MallocAllocator>())
{
    assert(instance == nullptr);
    instance = this;
}

template <typename T>
inline
ComponentsList<T>::~ComponentsList()
{
    assert(instance == this);
    assert(0 == pool.Count());
    instance = nullptr;
}

template <typename T>
inline void
ComponentsList<T>::OnUpdate()
{
	for (auto it = pool.Begin(), end = pool.End(); it < end; ++it) {
		switch (this->GetComponentState(it)) {
            case Component::Deserializing:
                break;
			case Component::Creating:
				it->OnCreate();

				if (it->IsActive()) {
					it->OnEnable();
					it->OnStart();

					this->SetComponentState(it, Component::Running);
				} else {
					this->SetComponentState(it, Component::Starting);
				}
				break;
			case Component::Starting:
				if (it->IsActive()) {
					it->OnEnable();
					it->OnStart();

					this->SetComponentState(it, Component::Running);
				}
				break;
			case Component::Running:
				if (it->IsActive())
					it->Update();
				else {
					it->OnDisable();

					this->SetComponentState(it, Component::Disabled);
				}
				break;
			case Component::Disabled:
				if (it->IsActive()) {
					it->OnEnable();
                    it->Update();

					this->SetComponentState(it, Component::Running);
				}
				break;
		}
	}
}

template <typename T>
inline void
ComponentsList<T>::OnLateUpdate()
{
	for (auto it = pool.Begin(), end = pool.End(); it < end; ++it) {
        if (Component::State::Deserializing == this->GetComponentState(it))
            continue;

        if (it->IsActive()) {
			assert(Component::Running == this->GetComponentState(it));
			it->LateUpdate();
		}
	}
}

template <typename T>
inline void
ComponentsList<T>::OnRender()
{
	for (auto it = pool.Begin(), end = pool.End(); it < end; ++it) {
        if (Component::State::Deserializing == this->GetComponentState(it))
            continue;
        
        if (it->IsActive()) {
			assert(Component::Running == this->GetComponentState(it));
			it->Render();
		}
	}
}

template <typename T>
inline void
ComponentsList<T>::OnPause()
{
    for (auto it = pool.Begin(), end = pool.End(); it < end; ++it) {
        if (Component::State::Deserializing == this->GetComponentState(it))
            continue;

        if (it->IsActive())
			it->OnApplicationPause();
	}
}

template <typename T>
inline void
ComponentsList<T>::OnResume()
{
    for (auto it = pool.Begin(), end = pool.End(); it < end; ++it) {
        if (Component::State::Deserializing == this->GetComponentState(it))
            continue;

        if (it->IsActive())
			it->OnApplicationResume();
	}
}

template <typename T>
inline void
ComponentsList<T>::OnQuit()
{
    for (auto it = pool.Begin(), end = pool.End(); it < end; ++it) {
        if (Component::State::Deserializing == this->GetComponentState(it))
            continue;

        if (it->IsActive())
			it->OnApplicationQuit();
	}
}

template <typename T>
inline Handle<Component>
ComponentsList<T>::NewComponentInstance(Entity *entity)
{
    return this->AttachToEntity(entity).template Cast<Component>();
}

template <typename T>
inline Handle<Component>
ComponentsList<T>::NewComponentToDeserialize(Entity *entity)
{
    Handle<T> p = pool.NewInstance();
    this->AttachComponentToEntity(entity, p);
    this->SetComponentState(p, Component::Deserializing);

    return p.template Cast<Component>();
}

template <typename T>
inline bool
ComponentsList<T>::TryDeserializeComponent(Component *component, SerializationServer *server, const BitStream &stream)
{
    if (component < this->Begin() || component >= this->End())
        return false;

    static_cast<T*>(component)->Deserialize(server, stream);
    this->SetComponentState(component, Component::Creating);

    return true;
}

template <typename T>
inline const ClassInfo*
ComponentsList<T>::GetComponentClassInfo() const
{
    return pool.GetBaseObjectClassInfo();
}

//#if defined(EDITOR)
template <typename T>
inline void
ComponentsList<T>::OnSerializeComponents(BitStream &stream)
{
    for (auto it = pool.Begin(), end = pool.End(); it < end; ++it) {
        if (this->GetComponentSerializationId(it) != kInvalidSerializationId)
            it->OnSerialize(stream);
    }
}
//#endif

} // namespace Framework
