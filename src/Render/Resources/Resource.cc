#include "Render/Resources/Resource.h"
#include "Render/Resources/ResourceServer.h"
#include "Core/Collections/List.h"
#include "Render/RenderQueue.h"

namespace Framework {

DefineAbstractClassInfo(Framework::Resource, Framework::Trackable);

Resource::Resource()
: size(0),
  isLoaded(false),
  access(ReadOnly),
  lastFrameUsed(-1)
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

bool
Resource::PrepareForRendering(RenderQueue *renderQueue)
{
    if (renderQueue->GetFrameCount() == lastFrameUsed)
        return false;

    renderQueue->RegisterResource(this);
    lastFrameUsed = renderQueue->GetFrameCount();

    return true;
}

} // namespace Framework
