#pragma once

#include "Core/Pool/BaseObject.h"
#include "Core/Pool/Handle.h"
#include "Core/Pool/Pool.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Collections/Array.h"
#include "Core/Pool/HandleList.h"
#include "Game/Component.h"
#include "Game/ComponentsList_type.h"
#include "Game/MsgDispatcher_type.h"
#include "Components/Transform.h"

namespace Framework {

class Renderer;
class SerializationServer;

//#if defined(EDITOR)
const uint32_t kInvalidSerializationId = 0xffffffff;
//#endif

class Entity : public BaseObject {
    DeclareClassInfo;
protected:
    HandleList<Component, &Component::entityNode> components;

	bool activeInHierarchy;
	bool branchActive;

    mutable Handle<Transform> transform;
    mutable Handle<Renderer> renderer;

    void SetActiveRecursively(bool flag);

	template <typename T> T* GetComponentRecursively() const;
	template <typename T> void GetComponentsRecursively(Array<Handle<T>> &components) const;

//#if defined(EDITOR)
    uint32_t serializationId;
    HandleListNode<Entity> serializationNode;

    uint32_t GetSerializationId() const;
    void MarkForSerialization(uint32_t &nextUniqueId);
    void Serialize(BitStream &stream);
    void ClearSerializationIds();
//#endif
    void Deserialize(SerializationServer *server, const BitStream &stream);
public:
    Entity();
    Entity(const Entity &other);
    Entity(Entity &&other);
    virtual ~Entity();

	bool IsActiveInHierarchy() const;
	bool IsActive() const;
	void SetActive(bool flag);

    const Handle<Transform>& GetTransform() const;
    const Handle<Renderer>& GetRenderer() const;

    template <typename T>
    Handle<T> AddComponent();

    template <typename T, typename... Args>
    Handle<T> AddComponent(Args... arguments);

    Handle<Component> AddComponent(const StringHash &className);

    template <typename T>
    Handle<T> GetComponent() const;

    template <typename T>
    Array<Handle<T>> GetComponents() const;

	template <typename T>
	Handle<T> GetComponentInChildren() const;

	template <typename T>
    Array<Handle<T>> GetComponentsInChildren(Allocator &allocator) const;

    template <typename... Args>
    void SendMessage(const StringHash &msg, Args... arguments);

    template <typename RetType, typename... Args>
    Array<RetType> SendMessage(const StringHash &msg, Args... arguments);

    void DestroyRecursively();

    friend class BaseComponentsList;
    friend class SerializationServer;
    friend class Transform;
    friend class Renderer;
};

template <typename T>
inline Handle<T>
Entity::AddComponent()
{
    return ComponentsList<T>::Instance()->AttachToEntity(this);
}

template <typename T, typename... Args>
inline Handle<T>
Entity::AddComponent(Args... arguments)
{
	return ComponentsList<T>::Instance()->AttachToEntity(this, arguments...);
}

template <typename T>
inline Handle<T>
Entity::GetComponent() const
{
    auto it = components.Begin();
    while (it != nullptr) {
        if (it->IsA<T>())
            return static_cast<T*>(const_cast<Component*>(it));

        it = components.GetNext(it);
    }
    return Handle<T>::Null;
}

template <typename T>
inline Array<Handle<T>>
Entity::GetComponents() const
{
    Array<Handle<T>> components(Memory::GetAllocator<MallocAllocator>());
    auto it = components.Begin();
    while (it != nullptr) {
        if (it->template IsA<T>())
            components.PushBack(static_cast<T*>(const_cast<Component*>(it)));

        it = components.GetNext(it);
    }
	return components;
}

template <typename T>
inline T*
Entity::GetComponentRecursively() const
{
	auto it = components.Begin();
	while (it != nullptr) {
		if (it->IsA<T>())
            return static_cast<T*>(const_cast<Component*>(it));

		it = components.GetNext(it);
	}

	auto child = transform->GetChildren().GetFirst();
	while (child.IsValid()) {
		T *ptr = child->GetEntity()->GetComponentRecursively<T>();
		if (ptr != nullptr)
			return ptr;

		child = transform->GetChildren().GetNext(child);
	}

    return nullptr;
}

template <typename T>
inline void
Entity::GetComponentsRecursively(Array<Handle<T>> &outComponents) const
{
	auto it = components.Begin();
	while (it != nullptr) {
		if (it->IsA<T>())
            outComponents.PushBack(static_cast<T*>(const_cast<Component*>(it)));

		it = components.GetNext(it);
	}

	auto child = transform->GetChildren().GetFirst();
	while (child.IsValid()) {
        child->GetEntity()->GetComponentsRecursively<T>(outComponents);

		child = transform->GetChildren().GetNext(child);
	}
}

template <typename T>
inline Handle<T>
Entity::GetComponentInChildren() const
{
	T *p = this->GetComponentRecursively<T>();
    if (nullptr == p)
        return Handle<T>::Null;
    else
        return p;
}

template <typename T>
inline Array<Handle<T>>
Entity::GetComponentsInChildren(Allocator &allocator) const
{
    Array<Handle<T>> components(allocator);
	this->GetComponentsRecursively<T>(components);
	return components;
}

template <typename... Args>
inline void
Entity::SendMessage(const StringHash &msg, Args... arguments)
{
    auto it = components.Begin();
    while (it != nullptr) {
        auto tmp = it;
        it = components.GetNext(it);

        MsgDispatcher<void(Args...)>::Instance()->Dispatch(msg, tmp, arguments...);
    }
}

template <typename RetType, typename... Args>
inline Array<RetType>
Entity::SendMessage(const StringHash &msg, Args... arguments)
{
    Array<RetType> a(Memory::GetAllocator<MallocAllocator>(), components.Count());
    auto it = components.Begin();
    while (it != nullptr) {
        auto tmp = it;
        it = components.GetNext(it);

        a.PushBack(MsgDispatcher<RetType(Args...)>::Instance()->Dispatch(msg, tmp, arguments...));
    }
    return a;
}

} // namespace Framework
