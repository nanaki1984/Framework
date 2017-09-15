#include "Render/Resources/ResourceServer.h"
#include "Core/Collections/Hash.h"
#include "Core/String.h"
#include "Core/StringHash.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/BlocksAllocator.h"

namespace Framework {

DefineClassInfo(Framework::ResourceServer, Framework::RefCounted);

WeakPtr<Resource>
ResourceServer::GetResourceById(ResourceId id)
{
    auto item = resourcesById.Get(id);
    if (nullptr == item)
        return nullptr;
    else
        return item->Get();
}

WeakPtr<Resource>
ResourceServer::GetResourceByName(const StringHash &name)
{
    auto item = resourcesByName.Get(name.hash);
    while (item != nullptr) {
        const char *resName = item->Get()->GetName();
        if (resName == name.string || 0 == strcmp(resName, name.string))
            break;
        item = resourcesByName.Next(item);
    }

    if (nullptr == item)
        return nullptr;
    else
        return item->Get();
}

WeakPtr<Resource>
ResourceServer::GetResourceByFilename(const char *filename)
{
    return this->GetResourceByName(this->GetResourceNameFromFilename(filename));
}

String
ResourceServer::GetResourceNameFromFilename(const char *filename)
{
    String str(filename);

    str.Replace(':', '_');
    str.Replace('/', '_');

    return str;
}

StringHash
ResourceServer::GetResourceCloneName(const StringHash &srcName)
{
    String cloneName(srcName.string);
    char counter = '0';
    cloneName.Append("-clone");
    cloneName.Append(counter++);

    while (this->GetResourceByName(cloneName) != nullptr) {
        cloneName.Clear();
        cloneName.Append(srcName.string);
        cloneName.Append("-clone");
        cloneName.Append(counter++);
    }

    return cloneName;
}

SmartPtr<Resource>
ResourceServer::CloneResource(const StringHash& name, Resource::Access access)
{
    assert(access != Resource::ReadOnly);
    Resource *res = this->GetResourceByName(name);
    assert(res != nullptr);

    SmartPtr<Resource> newRes = SmartPtr<Resource>::CastFrom(res->GetRTTI()->Create(&Memory::GetAllocator<BlocksAllocator>()));

    newRes->SetId(nextId++);
    newRes->SetName(this->GetResourceCloneName(name));
    newRes->SetAccess(access);
    newRes->SetCloneSource(res);

    resourcesById.Add(newRes->GetId(), newRes);
    resourcesByName.Add(newRes->GetHashCode(), newRes);

    newRes->Load();

    return newRes;
}

ResourceServer::ResourceServer()
: nextId(0),
  resourcesById(Memory::GetAllocator<MallocAllocator>()),
  resourcesByName(Memory::GetAllocator<MallocAllocator>())
{ }

ResourceServer::~ResourceServer()
{
    for (auto it = resourcesById.Begin(), end = resourcesById.End(); it != end; ++it) {
        if ((*it)->IsLoaded())
            (*it)->Unload();
    }
}

WeakPtr<Resource>
ResourceServer::NewResource(const ClassInfo &classInfo, const StringHash &name, Resource::Access access)
{
    assert(classInfo.IsDerivedFrom(&Resource::RTTI));

    Resource *res = this->GetResourceByName(name);

    if (res != nullptr) {
        if (Resource::ReadOnly == access)
            return res;
        else
            return this->CloneResource(name, access).Get();
    } else {
        SmartPtr<Resource> newRes = SmartPtr<Resource>::CastFrom(classInfo.Create(&Memory::GetAllocator<BlocksAllocator>()));

        newRes->SetId(nextId++);
        newRes->SetName(name);
        newRes->SetAccess(Resource::ReadOnly);
        newRes->SetCloneSource(nullptr);

        resourcesById.Add(newRes->GetId(), newRes);
        resourcesByName.Add(newRes->GetHashCode(), newRes);

        if (Resource::ReadOnly == access)
            return newRes.Get();
        else
            return this->CloneResource(name, access).Get();
    }
}

WeakPtr<Resource>
ResourceServer::NewResource(const StringHash &className, const StringHash &name, Resource::Access access)
{
    auto classInfo = ClassInfoUtils::Instance()->FindClass(className);
    if (classInfo != nullptr)
        return this->NewResource(*classInfo, name, access);
    else
        return nullptr;
}

WeakPtr<Resource>
ResourceServer::NewResourceFromFile(const ClassInfo &classInfo, const char *filename, Resource::Access access)
{
    StringHash name = this->GetResourceNameFromFilename(filename);

    Resource *newRes = this->NewResource(classInfo, name, access);
    if (!newRes->IsLoaded())
    {
        newRes->SetFilename(filename);
        newRes->Load();
    }

    return newRes;
}

WeakPtr<Resource>
ResourceServer::NewResourceFromFile(const StringHash &className, const char *filename, Resource::Access access)
{
    auto classInfo = ClassInfoUtils::Instance()->FindClass(className);
    if (classInfo != nullptr)
        return this->NewResourceFromFile(*classInfo, filename, access);
    else
        return nullptr;
}

void
ResourceServer::DestroyResource(Resource *resource)
{
    if (resource->IsLoaded())
        resource->Unload();

    auto item1 = resourcesById.Get(resource->GetId()),
         item2 = resourcesByName.Get(resource->GetHashCode());
    while (item2 != nullptr) {
        if (0 == strcmp((*item2)->GetName(), resource->GetName()))
            break;
        item2 = resourcesByName.Next(item2);
    }

    assert(item1 != nullptr && item2 != nullptr);

    resourcesById.Remove(item1);
    resourcesByName.Remove(item2);
}

void
ResourceServer::UnloadUnusedResources() const
{
    for (auto it = resourcesById.Begin(), end = resourcesById.End(); it != end; ++it) {
        if (0 == (*it)->GetWeakRefCount() && (*it)->IsLoaded())
            (*it)->Unload();
    }
}

} // namespace Framework
