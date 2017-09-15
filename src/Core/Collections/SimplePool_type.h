#pragma once

#include "Core/Collections/Array_type.h"
#include "Core/Collections/Queue_type.h"

namespace Framework {

template <typename T>
class SimplePool {
private:
    Array<T> data;
    Queue<uint32_t> indices;
public:
    SimplePool(Allocator &allocator);
    SimplePool(Allocator &allocator, uint32_t initialCapacity);
    SimplePool(const SimplePool<T> &other);
    SimplePool(SimplePool<T> &&other);
    ~SimplePool();

    SimplePool<T>& operator =(const SimplePool<T> &other);
    SimplePool<T>& operator =(SimplePool<T> &&other);

    Allocator& GetAllocator() const;
    uint32_t Count() const;
    uint32_t Capacity() const;
    bool IsEmpty() const;

    const T* Begin() const;
    T* Begin();
    const T* End() const;
    T* End();

    uint32_t Allocate();
    void Free(uint32_t index);

    const T& Get(uint32_t index) const;
    T& Get(uint32_t index);

    void Set(uint32_t index, const T &item);
};

} // namespace Framework
