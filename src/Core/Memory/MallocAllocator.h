#pragma once

#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"

namespace Framework {

class MallocAllocator : public Allocator {
    DeclareClassInfo;
    DeclareAllocator(MallocAllocator);
private:
    size_t totalAllocated;
public:
    MallocAllocator();
    virtual ~MallocAllocator();

    virtual void* Allocate(size_t size, size_t align);
    virtual void Free(void *pointer);

    virtual size_t GetAllocatedSize(void *pointer);
    virtual size_t GetTotalAllocated();
};

} // namespace Framework
