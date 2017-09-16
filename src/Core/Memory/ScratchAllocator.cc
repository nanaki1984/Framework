#include "Core/Memory/ScratchAllocator.h"

namespace Framework {

struct Header {
    uint32_t size;
};

DefineClassInfo(Framework::ScratchAllocator, Framework::Allocator);
DefineAllocator(Framework::ScratchAllocator);

bool
ScratchAllocator::InUse(uint8_t *pointer)
{
    if (free == allocated)
        return false;
    if (allocated > free)
        return pointer >= free && pointer < allocated;
    return pointer >= free || pointer < allocated;
}

ScratchAllocator::ScratchAllocator(Allocator *allocator, uint32_t size)
: baseAllocator(allocator),
  totalAllocated(0)
{
    begin = static_cast<uint8_t*>(baseAllocator->Allocate(size, 1));
    end = begin + size;
    allocated = free = begin;
}

ScratchAllocator::~ScratchAllocator()
{
    assert(free == allocated && 0 == totalAllocated);
    baseAllocator->Free(begin);
}

void*
ScratchAllocator::Allocate(size_t size, size_t align)
{
    //assert(0 == (align % 4));
    //align = ((align + 3) >> 2) << 2;
    void *d = nullptr;

    size_t ts = Allocator::GetAlignedSize<Header>(size, align);
    ts = ((ts + 3) / 4) * 4;

    // Request bigger than scratch buffer.
    if (ts > (uintptr_t(end) - uintptr_t(begin)))
        return baseAllocator->Allocate(size, align);

    uint8_t *p = allocated;
    Header *h = (Header*)p;
    d = Allocator::GetDataFromPointer<Header>(h, align);

    p += ts;

    // Reached the end of the buffer, wrap around to the beginning. (mark free last block)
    if (p > end)
    {
        h->size = (uintptr_t(end) - uintptr_t(h)) | 0x80000000u;

        p = begin;
        h = (Header*)p;
        d = Allocator::GetDataFromPointer<Header>(h, align);

        p += ts;
    }

    // If the buffer is exhausted use the backing allocator instead.
    if (this->InUse(p))
        return baseAllocator->Allocate(size, align);

    h->size = ts;
    Allocator::FillPadding(h, d);

    if (p == end)
        p = begin;
    allocated = (uint8_t*)p;

    totalAllocated += ts;

    return d;
}

void
ScratchAllocator::Free(void *pointer)
{
    if (nullptr == pointer)
        return;

    if (pointer < begin || pointer >= end)
    {
        baseAllocator->Free(pointer);
        return;
    }

    // Mark this slot as free
    Header *h = Allocator::GetPointerFromData<Header>(pointer);
    assert((h->size & 0x80000000u) == 0);
    totalAllocated -= h->size;
    h->size = h->size | 0x80000000u;

    // Advance the free pointer past all free slots.
    while (free != allocated) {
        Header *h = (Header*)free;
        if ((h->size & 0x80000000u) == 0)
            break;

        free += h->size & 0x7fffffffu;
        if (free == end)
            free = begin;
    }
}

size_t
ScratchAllocator::GetAllocatedSize(void *pointer)
{
    return Allocator::GetPointerFromData<Header>(pointer)->size;
}

size_t
ScratchAllocator::GetTotalAllocated()
{
    return totalAllocated;
}

} // namespace Framework
