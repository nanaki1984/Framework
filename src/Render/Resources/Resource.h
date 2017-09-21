#pragma once

#include "Core/Trackable.h"
#include "Core/WeakPtr.h"
#include "Core/String.h"
#include "Core/Collections/List_type.h"
#include "Render/Resources/ResourceId.h"
#include "Render/Resources/ResourceName.h"

namespace Framework {

class RenderQueue;

class Resource : public Trackable {
    DeclareClassInfo;
public:
    enum Access {
        ReadOnly = (1 << 0),
        Writable = (1 << 1)
    };
protected:
    uint32_t size;
    bool     isLoaded;

    ResourceId   id;
    ResourceName name;
    String		 filename;

    WeakPtr<Resource> cloneSource;
    Access            access;

    ListNode<Resource> node;
    int                lastFrameUsed;

    virtual bool LoadImpl() = 0;
    virtual void UnloadImpl() = 0;
    virtual bool CloneImpl(WeakPtr<Resource> source) = 0;
    virtual uint32_t ComputeSize() = 0;
public:
    typedef List<Resource, &Resource::node> IntrusiveList;

    Resource();
    virtual ~Resource();

    uint32_t GetSize() const;
    float GetTimeStamp() const;
    virtual uint32_t GetAccessModes() const = 0;

    Access GetAccess() const;
    void SetAccess(Access a);

    const WeakPtr<Resource>& GetCloneSource() const;
    void SetCloneSource(Resource *res);

    ResourceId GetId() const;
    void SetId(ResourceId _id);

    const char* GetName() const;
    uint32_t GetHashCode() const;
    void SetName(const ResourceName &_name);

    const String& GetFilename() const;
    void SetFilename(const String &_filename);

    bool IsLoaded() const;

    bool Load();
    void Unload();
    WeakPtr<Resource> Clone();

    bool PrepareForRendering(RenderQueue *renderQueue);
    virtual void OnEndFrameCommands() = 0;
};

inline uint32_t
Resource::GetSize() const
{
    return size;
}

inline Resource::Access
Resource::GetAccess() const
{
    return access;
}

inline void
Resource::SetAccess(Resource::Access mode)
{
    assert(mode & this->GetAccessModes());
    access = mode;
}

inline const WeakPtr<Resource>&
Resource::GetCloneSource() const
{
    return cloneSource;
}

inline void
Resource::SetCloneSource(Resource *res)
{
    cloneSource = res;
}

inline ResourceId
Resource::GetId() const
{
    return id;
}

inline void
Resource::SetId(ResourceId _id)
{
    id = _id;
}

inline const char*
Resource::GetName() const
{
    return name.string;
}

inline uint32_t
Resource::GetHashCode() const
{
    return name.hash;
}

inline void
Resource::SetName(const ResourceName &_name)
{
    name = _name;
}

inline const String&
Resource::GetFilename() const
{
    return filename;
}

inline void
Resource::SetFilename(const String &_filename)
{
    filename = _filename;
}

inline bool
Resource::IsLoaded() const
{
    return isLoaded;
}

} // namespace Framework
