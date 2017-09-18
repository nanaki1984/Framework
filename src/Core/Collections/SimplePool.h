#pragma once

#include "Core/Collections/SimplePool_type.h"
#include "Core/Collections/Array.h"
#include "Core/Collections/Queue.h"

namespace Framework {

template <typename T>
inline
SimplePool<T>::SimplePool(Allocator &allocator)
: data(allocator),
  indices(allocator)
{ }

template <typename T>
inline
SimplePool<T>::SimplePool(Allocator &allocator, uint32_t initialCapacity)
: data(allocator, initialCapacity),
  indices(allocator, initialCapacity)
{ }

template <typename T>
inline
SimplePool<T>::SimplePool(const SimplePool<T> &other)
: data(other.data),
  indices(other.indices)
{ }

template <typename T>
inline
SimplePool<T>::SimplePool(SimplePool<T> &&other)
: data(std::forward<Array<T>>(other.data)),
  indices(std::forward<Queue<uint32_t>>(other.indices))
{ }

template <typename T>
inline
SimplePool<T>::~SimplePool()
{ }

template <typename T>
inline SimplePool<T>&
SimplePool<T>::operator =(const SimplePool<T> &other)
{
    data = other.data;
    indices = other.indices;
}

template <typename T>
inline SimplePool<T>&
SimplePool<T>::operator =(SimplePool<T> &&other)
{
	data = std::forward<Array<T>>(other.data);
	indices = std::forward<Queue<uint32_t>>(other.indices);
    return (*this);
}

template <typename T>
inline Allocator&
SimplePool<T>::GetAllocator() const
{
    return data.GetAllocator();
}

template <typename T>
inline uint32_t
SimplePool<T>::Count() const
{
    return data.Count() - indices.Count();
}

template <typename T>
inline uint32_t
SimplePool<T>::Capacity() const
{
    return data.Capacity();
}

template <typename T>
inline bool
SimplePool<T>::IsEmpty() const
{
    return data.IsEmpty();
}

template <typename T>
inline const T*
SimplePool<T>::Begin() const
{
    return data.Begin();
}

template <typename T>
inline T*
SimplePool<T>::Begin()
{
    return data.Begin();
}

template <typename T>
inline const T*
SimplePool<T>::End() const
{
    return data.End();
}

template <typename T>
inline T*
SimplePool<T>::End()
{
    return data.End();
}

template <typename T>
inline uint32_t
SimplePool<T>::Allocate()
{
    if (indices.IsEmpty()) {
        data.PushBack(T());
        return data.Count() - 1;
    } else {
        assert(indices.Front() < data.Count());
        uint32_t index = indices.Front();
        indices.PopFront();
        return index;
    }
}

template <typename T>
inline void
SimplePool<T>::Free(uint32_t index)
{
    assert(index < data.Count());
    indices.PushBack(index);
}

template <typename T>
inline const T&
SimplePool<T>::Get(uint32_t index) const
{
    return data[index];
}

template <typename T>
inline T&
SimplePool<T>::Get(uint32_t index)
{
    return data[index];
}

template <typename T>
inline void
SimplePool<T>::Set(uint32_t index, const T &item)
{
    data[index] = item;
}

} // namespace Framework
