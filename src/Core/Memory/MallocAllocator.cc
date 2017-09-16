#include "Core/Memory/MallocAllocator.h"

namespace Framework {

struct Header {
    uint32_t size;
};

DefineClassInfo(Framework::MallocAllocator, Framework::Allocator);
DefineAllocator(Framework::MallocAllocator);

MallocAllocator::MallocAllocator()
: totalAllocated(0)
{ }

MallocAllocator::~MallocAllocator()
{
    assert(0 == totalAllocated);
}

void*
MallocAllocator::Allocate(size_t size, size_t align)
{
    void *d = nullptr;

    size_t ts = Allocator::GetAlignedSize<Header>(size, align);
    Header *p = (Header*)malloc(ts);
    d = Allocator::GetDataFromPointer<Header>(p, align);
    p->size = ts;
    totalAllocated += ts;

    Allocator::FillPadding(p, d);

    return d;
}

void
MallocAllocator::Free(void *pointer)
{
    if (nullptr == pointer)
        return;

    Header *h = Allocator::GetPointerFromData<Header>(pointer);
    totalAllocated -= h->size;
    free(h);
}

size_t
MallocAllocator::GetAllocatedSize(void *pointer)
{
    return Allocator::GetPointerFromData<Header>(pointer)->size;
}

size_t
MallocAllocator::GetTotalAllocated()
{
    return totalAllocated;
}

} // namespace Framework
