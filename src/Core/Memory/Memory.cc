#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"
#include "Core/Collections/List.h"

namespace Framework {
    namespace Memory {

char __allocBuffer[1024];
char *__allocPointer;
const char *__allocEnd = __allocBuffer + sizeof(__allocBuffer);
List<Allocator, &Allocator::node> allocators;

void InitializeMemory()
{
    __allocPointer = __allocBuffer;
}

void ShutdownMemory()
{
    Allocator *node = allocators.End();
    while (node != nullptr) {
        node->~Allocator();
        node = allocators.GetPrevious(node);
    }
    allocators.Clear();
}

void* GetAllocatorMemory(size_t size)
{
    assert(size_t(__allocEnd - __allocPointer) >= size);
    void *p = static_cast<void*>(__allocPointer);
    __allocPointer += size;
    return  p;
}

void InitAllocatorMemory(Allocator *pointer)
{
    allocators.PushBack(pointer);
}

    } // namespace Memory
} // namespace Framework
