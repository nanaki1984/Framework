#include "Core/RefCounted.h"
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
        GC.garbage.PushBack(this);
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

GarbageCollector::GarbageCollector()
{ }

GarbageCollector::~GarbageCollector()
{ }

void
GarbageCollector::Collect()
{
    RefCounted *node = garbage.Begin();
    while (node != nullptr) {
        garbage.PopFront();

        node->~RefCounted();
        assert(node->allocator != nullptr);
        node->allocator->Free(node);

        node = garbage.Begin();
    }
}

}; // namespace Framework
