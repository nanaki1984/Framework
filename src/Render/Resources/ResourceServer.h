#pragma once

#include "Core/Singleton.h"
#include "Core/Collections/Hash_type.h"
#include "Render/Resources/Resource.h"
#include "Core/SmartPtr.h"
#include "Core/WeakPtr.h"

namespace Framework {

class String;
struct StringHash;

class ResourceServer : public Singleton<ResourceServer> {
    DeclareClassInfo;
private:
    ResourceId nextId;
    Hash<SmartPtr<Resource>> resourcesById;
    Hash<SmartPtr<Resource>> resourcesByName;

    String GetResourceNameFromFilename(const char *filename);
    StringHash GetResourceCloneName(const StringHash &srcName);
    SmartPtr<Resource> CloneResource(const StringHash& name, Resource::Access access);
public:
    ResourceServer();
    ResourceServer(const ResourceServer &other) = delete;
    virtual ~ResourceServer();

    ResourceServer& operator =(const ResourceServer &other) = delete;

    WeakPtr<Resource> GetResourceById(ResourceId id);
    WeakPtr<Resource> GetResourceByName(const StringHash &name);
    WeakPtr<Resource> GetResourceByFilename(const char *filename);

    WeakPtr<Resource> NewResource(const ClassInfo &classInfo, const StringHash &name, Resource::Access access);
    WeakPtr<Resource> NewResource(const StringHash &className, const StringHash &name, Resource::Access access);
    WeakPtr<Resource> NewResourceFromFile(const ClassInfo &classInfo, const char *filename, Resource::Access access);
    WeakPtr<Resource> NewResourceFromFile(const StringHash &className, const char *filename, Resource::Access access);

    template <typename T>
    WeakPtr<T> NewResource(const StringHash &name, Resource::Access access);
    template <typename T>
    WeakPtr<T> NewResourceFromFile(const char *filename, Resource::Access access);

    void DestroyResource(Resource *resource);

    void UnloadUnusedResources() const;
};

template <typename T>
WeakPtr<T>
ResourceServer::NewResource(const StringHash &name, Resource::Access access)
{
    return this->NewResource(T::RTTI, name, access).template Cast<T>();
}

template <typename T>
WeakPtr<T>
ResourceServer::NewResourceFromFile(const char *filename, Resource::Access access)
{
    return this->NewResourceFromFile(T::RTTI, filename, access).template Cast<T>();
}

} // namespace Framework
