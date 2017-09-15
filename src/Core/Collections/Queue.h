#pragma once

#include "Core/Collections/Queue_type.h"
#include "Core/Collections/Array.h"

namespace Framework {

template <typename T>
inline void
Queue<T>::Grow(uint32_t minCapacity)
{
    uint32_t newCapacity = (data.Capacity() * 2) + 8;
    if (newCapacity < minCapacity)
        newCapacity = minCapacity;

    uint32_t prevCapacity = data.Capacity();
    data.Resize(newCapacity);
    if ((offset + size) >= prevCapacity) {
        uint32_t nItemsEnd = prevCapacity - offset;
        Memory::MoveReverse(data.Begin() + newCapacity - nItemsEnd, data.Begin() + prevCapacity - nItemsEnd, nItemsEnd);
        offset += (newCapacity - prevCapacity);
    }
}

template <typename T>
inline
Queue<T>::Queue(Allocator &allocator)
: size(0),
  offset(0),
  data(allocator)
{ }

template <typename T>
inline
Queue<T>::Queue(Allocator &allocator, uint32_t initialCapacity)
: size(0),
  offset(0),
  data(allocator, initialCapacity)
{
    data.Resize(initialCapacity);
}

template <typename T>
inline
Queue<T>::Queue(const Queue<T> &other)
: size(other.size),
  offset(other.offset),
  data(other.data)
{ }

template <typename T>
inline
Queue<T>::Queue(Queue<T> &&other)
: size(other.size),
  offset(other.offset),
  data(std::forward<Array<T>>(other.data))
{
    other.size = 0;
    other.offset = 0;
}

template <typename T>
inline
Queue<T>::~Queue()
{ }

template <typename T>
inline Queue<T>&
Queue<T>::operator =(const Queue<T> &other)
{
    size = other.size;
    offset = other.offset;
    data = other.data;
    return (*this);
}

template <typename T>
inline Queue<T>&
Queue<T>::operator =(Queue<T> &&other)
{
    size = other.size;
    offset = other.offset;
    data = std::forward<Array<T>>(other.data);
    other.size = 0;
    other.offset = 0;
    return (*this);
}

template <typename T>
inline const T&
Queue<T>::operator [](uint32_t index) const
{
    return data[(offset + index) % data.Capacity()];
}

template <typename T>
inline T&
Queue<T>::operator [](uint32_t index)
{
    return data[(offset + index) % data.Capacity()];
}

template <typename T>
inline Allocator&
Queue<T>::GetAllocator() const
{
    return data.GetAllocator();
}

template <typename T>
inline uint32_t
Queue<T>::Count() const
{
    return size;
}

template <typename T>
inline uint32_t
Queue<T>::Capacity() const
{
    return data.Capacity();
}

template <typename T>
inline bool
Queue<T>::IsEmpty() const
{
    return 0 == size;
}

template <typename T>
inline const T&
Queue<T>::Front() const
{
    assert(size > 0);
    return (*this)[0];
}

template <typename T>
inline T&
Queue<T>::Front()
{
    assert(size > 0);
    return (*this)[0];
}

template <typename T>
inline const T&
Queue<T>::Back() const
{
    assert(size > 0);
    return (*this)[size - 1];
}

template <typename T>
inline T&
Queue<T>::Back()
{
    assert(size > 0);
    return (*this)[size - 1];
}

template <typename T>
inline void
Queue<T>::Clear()
{
    size = 0;
    offset = 0;
    uint32_t capacity = data.Capacity();
    data.Clear();
    data.Resize(capacity);
}

template <typename T>
inline void
Queue<T>::Reserve(uint32_t newCapacity)
{
    if (data.Capacity() < newCapacity)
        this->Grow(newCapacity);
}

template <typename T>
inline void
Queue<T>::PushBack(const T &item)
{
    if (data.Capacity() == size)
        this->Grow();
    Memory::Copy(&(*this)[size++], &item, 1);
}

template <typename T>
inline void
Queue<T>::PopBack()
{
    assert(size > 0);
    --size;
    if (!std::is_trivially_copyable<T>::value && std::is_destructible<T>::value)
        (data.Begin() + (offset + size) % data.Capacity())->~T();
}

template <typename T>
inline void
Queue<T>::PushFront(const T& item)
{
    if (data.Capacity() == size)
        this->Grow();
    offset = (offset - 1 + data.Capacity()) % data.Capacity();
    Memory::Copy(&(*this)[0], &item, 1);
    ++size;
}

template <typename T>
inline void
Queue<T>::PopFront()
{
    assert(size > 0);
    if (!std::is_trivially_copyable<T>::value && std::is_destructible<T>::value)
        (data.Begin() + offset)->~T();
    offset = (offset + 1) % data.Capacity();
    --size;
}

} // namespace Framework
