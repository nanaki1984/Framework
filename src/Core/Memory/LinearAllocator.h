#pragma once

#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"
#include "Core/Collections/Array_type.h"

namespace Framework {
        
class LinearAllocator : public Allocator {
    DeclareClassInfo;
    DeclareAllocator(LinearAllocator);
private:
    struct State {
        uint8_t *ptr;
#ifdef _DEBUG
        uint32_t allocCount;
        uint32_t totalAllocated;
#endif
    };

    Allocator *baseAllocator;

    uint8_t *begin;
    uint8_t *end;
    uint8_t *ptr;

    Array<State> stack;
public:
    LinearAllocator(Allocator *allocator, size_t bufferSize, size_t stackSize);
    virtual ~LinearAllocator();

    virtual void* Allocate(size_t size, size_t align);
    virtual void Free(void *pointer);

    virtual size_t GetAllocatedSize(void *pointer);
    virtual size_t GetTotalAllocated();

    void PushState();
    void PopState();
    void Reset();
};
        
} // namespace Framework
