#include "Core/Memory/LinearAllocator.h"
#include "Core/Debug.h"
#include "Core/Collections/Array.h"

namespace Framework {

DefineClassInfo(Framework::LinearAllocator, Framework::Allocator);
DefineAllocator(Framework::LinearAllocator);

LinearAllocator::LinearAllocator(Allocator *allocator, size_t bufferSize, size_t stackSize)
: baseAllocator(allocator),
  stack(*allocator, stackSize)
{
    begin = static_cast<uint8_t*>(baseAllocator->Allocate(bufferSize, 1));
    end   = begin + bufferSize;
    ptr   = begin;

    this->PushState();
}

LinearAllocator::~LinearAllocator()
{
    this->PopState();

    baseAllocator->Free(begin);
}

void*
LinearAllocator::Allocate(size_t size, size_t align)
{
    size_t ts = Allocator::GetAlignedSize(size, align);
    assert(end - ptr >= ts);

    void *p = ptr;
    ptr += ts;
    void *d = Allocator::GetDataFromPointer(p, align);
#ifdef _DEBUG
    assert(!stack.IsEmpty());
    for (State *it = stack.Begin(), *end = stack.End(); it < end; ++it) {
        ++it->allocCount;
        it->totalAllocated += ts;
    }
#endif
    return d;
}

void
LinearAllocator::Free(void *pointer)
{
    if (nullptr == pointer)
        return;
#ifdef _DEBUG
    assert(!stack.IsEmpty());
    for (State *it = stack.Begin(), *end = stack.End(); it < end; ++it)
        --it->allocCount;
#endif
}

size_t
LinearAllocator::GetAllocatedSize(void *pointer)
{
    return 0;
}

size_t
LinearAllocator::GetTotalAllocated()
{
#ifdef _DEBUG
    assert(!stack.IsEmpty())
    return stack.Front().totalAllocated;
#else
    return 0;
#endif
}

void
LinearAllocator::PushState()
{
#ifdef _DEBUG
    State state = { ptr, 0, 0 };
#else
    State state = { ptr };
#endif
    stack.PushBack(state);
}

void
LinearAllocator::PopState()
{
#ifdef _DEBUG
    assert(0 == stack.Back().allocCount);
    uint32_t ta = stack.Back().totalAllocated;
#endif
    ptr = stack.Back().ptr;
    stack.PopBack();
#ifdef _DEBUG
    if (!stack.IsEmpty()) {
        for (State *it = stack.Begin(), *end = stack.End(); it < end; ++it)
            it->totalAllocated -= ta;
    }
#endif
}

void
LinearAllocator::Reset()
{
    ptr = begin;
    stack.Clear();
    this->PushState();
}

} // namespace Framework
