#pragma once

#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"

namespace Framework {

class ScratchAllocator : public Allocator {
    DeclareClassInfo;
    DeclareAllocator(ScratchAllocator);
private:
    Allocator *baseAllocator;
    size_t totalAllocated;

    uint8_t *begin, *end;
    uint8_t *allocated, *free;

    bool InUse(uint8_t *pointer);
public:
    ScratchAllocator(Allocator *allocator, uint32_t size);
    virtual ~ScratchAllocator();

    virtual void* Allocate(size_t size, size_t align);
    virtual void Free(void *pointer);

    virtual size_t GetAllocatedSize(void *pointer);
    virtual size_t GetTotalAllocated();
};

} // namespace Framework
