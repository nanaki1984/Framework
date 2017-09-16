#include "Core/RefCounted.h"
#include "Core/Trackable.h"
#include "Core/Memory/Allocator.h"
#include "Core/Collections/List.h"

namespace Framework {

DefineRootClassInfo(Framework::RefCounted)

GarbageCollector RefCounted::GC;

RefCounted::RefCounted()
: refCount(0),
  allocator(nullptr)
{
    GC.garbage.PushBack(this);
}

RefCounted::~RefCounted()
{
    assert(0 == refCount);
}

void
RefCounted::AddRef()
{
    if (0 == refCount++)
        GC.garbage.Remove(this);
}

void
RefCounted::Release()
{
    int cnt = refCount--;
    assert(cnt > 0);

	if (1 == cnt)
	{
		this->InvalidateWeakReferences();

		GC.garbage.PushBack(this);
	}
}

uint32_t
RefCounted::GetRefCount() const
{
    return refCount;
}

bool
RefCounted::IsAlive() const
{
    return refCount > 0;
}

void
RefCounted::InvalidateWeakReferences()
{ }

GarbageCollector::GarbageCollector()
{ }

GarbageCollector::~GarbageCollector()
{ }

void
GarbageCollector::Collect()
{
    RefCounted *node = garbage.Begin();
    while (node != nullptr)
	{
		if (!node->IsA<Trackable>() || 0 == static_cast<Trackable*>(node)->GetWeakRefCount())
		{
			auto tmp = node;
			node = garbage.GetNext(node);
			garbage.Remove(tmp);

			tmp->~RefCounted();
			assert(tmp->allocator != nullptr);
			tmp->allocator->Free(tmp);
		}
		else
			node = garbage.GetNext(node);
	}
}

}; // namespace Framework
