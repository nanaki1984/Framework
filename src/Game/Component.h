#pragma once

#include "Core/Pool/BaseObject.h"
#include "Core/Pool/HandleList_type.h"

#define DeclareComponent \
public: \
    static int ExecutionOrder; \
    static Framework::BaseComponentsList *BaseList; \
 \
    virtual Framework::BaseComponentsList* GetBaseList() const; \
 \
    static void EnsureLinking();

#define DefineComponent(type,execOrder) \
    int type::ExecutionOrder = execOrder | type::RTTI.SetStaticCtor([]{ type::BaseList = Framework::ComponentsList<type>::Instance(); }); \
    Framework::BaseComponentsList *type::BaseList = nullptr; \
 \
    Framework::BaseComponentsList* type::GetBaseList() const \
    { \
        return this->BaseList; \
    } \
 \
     void type::EnsureLinking() { type::ExecutionOrder |= 0; }

namespace Framework {

class Entity;
class BaseComponentsList;
class SerializationServer;
class BitStream;

class Component : public BaseObject {
    DeclareClassInfo;
    DeclareComponent;
public:
    enum State {
        Deserializing = -1,

        Creating = 0,
        Starting,
        Running,
		Disabled
    };
protected:
    Handle<Entity> entity;
    State state;

	bool activeInEntity;
	bool entityActive;

    HandleListNode<Component> entityNode;

    Component* CloneTo(Entity *otherEntity) const;

//#if defined(EDITOR)
    uint32_t serializationId;

    uint32_t GetSerializationId() const;
    void MarkForSerialization(uint32_t &nextUniqueId);
    void ClearSerializationId();
//#endif
public:
    Component();
    Component(const Component &other);
    Component(Component &&other);
    virtual ~Component();

    const Handle<Entity>& GetEntity() const;

	bool IsActive() const;
	bool IsActiveInEntity() const;
	void SetActiveInEntity(bool flag);

    void Deserialize(SerializationServer *server, const BitStream &stream);
    void OnCreate();
    void OnStart();
    void Update();
    void LateUpdate();
	void Render();
	void OnEnable();
	void OnDisable();
    void OnApplicationPause();
    void OnApplicationResume();
    void OnApplicationQuit();

//#if defined(EDITOR)
    void OnSerialize(BitStream &stream);
//#endif

    friend class BaseComponentsList;
    friend class Entity;
};

} // namespace Game
