#pragma once

#include <cstdint>
#include <cstdlib>

#include "Core/ClassInfo.h"
#include "Core/Collections/List_type.h"

namespace Framework {

const uint8_t kPaddingValue = 0xfe;

class Allocator {
    DeclareRootClassInfo;
public:
    ListNode<Allocator> node;

    Allocator();
    virtual ~Allocator();

    virtual void* Allocate(size_t size, size_t align) = 0;
    virtual void Free(void *pointer) = 0;

    virtual size_t GetAllocatedSize(void *pointer) = 0;
    virtual size_t GetTotalAllocated() = 0;
protected:
    template <typename H>
    static H* GetPointerFromData(void *data)
    {
        uint8_t *padding = (uint8_t*)data;
        while (*(--padding) == kPaddingValue);
        return (H*)(uintptr_t(padding + 1) - sizeof(H));
    }

    template <typename H>
    static void* GetDataFromPointer(H *pointer, size_t align)
    {
        uintptr_t d = uintptr_t(pointer) + sizeof(H);
        if (0 == (d % align))
            return (void*)d;
        else
            return (void*)((d + align - 1) & ~(align - 1));
    }

    template <typename H>
    static void FillPadding(H *header, void *data)
    {
        uint8_t *padding = (uint8_t*)(uintptr_t(header) + sizeof(H));
        while (padding < data)
            *(padding++) = kPaddingValue;
    }

    template <typename H>
    static size_t GetAlignedSize(size_t size, size_t align)
    {
        return sizeof(H) + size + align - 1;
    }

    static inline void* GetPointerFromData(void *data)
    {
        uint8_t *padding = (uint8_t*)data;
        while (*(--padding) == kPaddingValue);
        return (void*)(padding + 1);
    }

    static inline void* GetDataFromPointer(void *pointer, size_t align)
    {
        uintptr_t d = uintptr_t(pointer);
        if (0 == (d % align))
            return (void*)d;
        else
            return (void*)((d + align - 1) & ~(align - 1));
    }

    static inline void FillPadding(void *pointer, void *data)
    {
        uint8_t *padding = (uint8_t*)pointer;
        while (padding < data)
            *(padding++) = kPaddingValue;
    }

    static inline size_t GetAlignedSize(size_t size, size_t align)
    {
        return size + align - 1;
    }
private:
    Allocator(const Allocator &other);
    Allocator& operator =(const Allocator &other);
};

    namespace Memory {

void* GetAllocatorMemory(size_t size);
void InitAllocatorMemory(Allocator *pointer);

template <typename A, typename ...Args> void InitAllocator(Args... params)
{
    A::__instance = static_cast<A*>(Memory::GetAllocatorMemory(sizeof(A)));
    new(A::__instance) A(params...);
    Memory::InitAllocatorMemory(A::__instance);
}

template <typename A> A& GetAllocator()
{
    return *A::__instance;
}

    } // namespace Memory
} // namespace Framework

#define DeclareAllocator(_Type) \
public: \
    static _Type *__instance;

#define DefineAllocator(_Type) \
    _Type *_Type::__instance = nullptr;
