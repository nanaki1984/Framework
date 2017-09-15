#include "Render/Resources/Resource.h"
#include "Render/Resources/ResourceServer.h"

namespace Framework {

DefineAbstractClassInfo(Framework::Resource, Framework::Trackable);

Resource::Resource()
: size(0),
  isLoaded(false),
  access(ReadOnly)
{ }

Resource::~Resource()
{
    assert(!isLoaded);
}

bool
Resource::Load()
{
    assert(!isLoaded);
    if (filename.IsEmpty())
        return isLoaded;

    if (ReadOnly == access) {
        isLoaded = this->LoadImpl();

        if (isLoaded)
            size = this->ComputeSize();
    } else {
        if (!cloneSource->IsLoaded())
            cloneSource->Load();

        isLoaded = this->CloneImpl(cloneSource);

        if (isLoaded)
            size = this->ComputeSize();
    }

    return isLoaded;
}

void
Resource::Unload()
{
    assert(isLoaded);

    this->UnloadImpl();
    size = 0;

    isLoaded = false;
}

WeakPtr<Resource>
Resource::Clone()
{
    if (filename.IsEmpty())
        return ResourceServer::Instance()->NewResource(*this->GetRTTI(), name, Writable);
    else
        return ResourceServer::Instance()->NewResourceFromFile(*this->GetRTTI(), this->GetFilename().AsCString(), Writable);
}

} // namespace Framework
