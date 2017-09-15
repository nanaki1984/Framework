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
    if (0 == refCount)
        GC.garbage.Remove(this);

    ++refCount;
}

void
RefCounted::Release()
{
    assert(refCount > 0);

    --refCount;

    if (0 == refCount)
        GC.garbage.PushBack(this);
}

uint32_t
RefCounted::GetRefCount() const
{
    return refCount;
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
