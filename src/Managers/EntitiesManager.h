#pragma once

#include "Managers/BaseManager.h"
#include "Core/StringHash.h"
#include "Core/Pool/Pool_type.h"

namespace Framework {

class Entity;
class SerializationServer;

class EntitiesManager : public BaseManager {
	DeclareClassInfo;
    DeclareManager;
protected:
	Pool<Entity> entities;

    Handle<Entity> CloneRecursively(Handle<Entity> source, Handle<Entity> cloneParent);
    Handle<Entity> NewVoidEntity();
public:
    EntitiesManager();
	virtual ~EntitiesManager();

	Handle<Entity> NewEntity();
	Handle<Entity> NewEntity(const StringHash &name);
	Handle<Entity> NewEntity(const StringHash &name, const Handle<Entity> &parent);

	Handle<Entity> CloneEntity(const Handle<Entity> &source, bool cloneRecursivelty = true);
    Handle<Entity> CloneEntity(const Handle<Entity> &source, const StringHash &cloneName, bool cloneRecursivelty = true);
    Handle<Entity> CloneEntity(const Handle<Entity> &source, const StringHash &cloneName, const Handle<Entity> &cloneParent, bool cloneRecursivelty = true);

    virtual void OnUpdate();
	virtual void OnLateUpdate();
	virtual void OnRender();
	virtual void OnPause();
	virtual void OnResume();
	virtual void OnQuit();

    friend class SerializationServer;
};

} // namespace Framework
