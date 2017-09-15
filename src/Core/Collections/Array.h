#pragma once

#include <algorithm>
#include "Core/Collections/Array_type.h"
#include "Core/Debug.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"

namespace Framework {

template <typename T>
inline void
Array<T>::Grow(uint32_t minCapacity)
{
    uint32_t newCapacity = capacity * 2 + 8;
    if (newCapacity < minCapacity)
        newCapacity = minCapacity;
    this->SetCapacity(newCapacity);
}

template <typename T>
inline
Array<T>::Array(Allocator &_allocator)
: size(0),
  capacity(0),
  data(nullptr),
  allocator(&_allocator)
{ }

template <typename T>
inline
Array<T>::Array(Allocator &_allocator, uint32_t initialCapacity)
: size(0),
  capacity(initialCapacity),
  allocator(&_allocator)
{
    data = static_cast<T*>(allocator->Allocate(sizeof(T) * capacity, __alignof(T)));
}

template <typename T>
inline
Array<T>::Array(const Array<T> &other)
: size(other.size),
  capacity(other.capacity),
  allocator(other.allocator)
{
    data = static_cast<T*>(allocator->Allocate(sizeof(T) * capacity, __alignof(T)));
    
    Memory::Copy(data, other.data, size);
}

template <typename T>
inline
Array<T>::Array(Array<T> &&other)
: size(other.size),
  capacity(other.capacity),
  data(other.data),
  allocator(other.allocator)
{
    other.size = 0;
    other.capacity = 0;
    other.data = nullptr;
}

template <typename T>
inline
Array<T>::~Array()
{
    this->Clear();
    if (data != nullptr)
        allocator->Free(data);

    data = nullptr;
}

template <typename T>
inline Array<T>&
Array<T>::operator =(const Array<T> &other)
{
    this->Clear();
    if (data != nullptr)
        allocator->Free(data);

    size = other.size;
    capacity = other.capacity;
    data = static_cast<T*>(other.allocator->Allocate(sizeof(T) * capacity, __alignof(T)));
    allocator = other.allocator;

    Memory::Copy(data, other.data, size);

    return (*this);
}

template <typename T>
inline Array<T>&
Array<T>::operator =(Array<T> &&other)
{
    this->Clear();
    if (data != nullptr)
        allocator->Free(data);

    size = other.size;
    capacity = other.capacity;
    data = other.data;
    allocator = other.allocator;
    
    other.size = 0;
    other.capacity = 0;
    other.data = nullptr;
    
    return (*this);
}

template <typename T>
inline const T&
Array<T>::operator [](uint32_t index) const
{
    assert(index < size);
    return data[index];
}

template <typename T>
inline T&
Array<T>::operator [](uint32_t index)
{
    assert(index < size);
    return data[index];
}

template <typename T>
Allocator&
Array<T>::GetAllocator() const
{
    return *const_cast<Allocator*>(allocator);
}

template <typename T>
inline uint32_t
Array<T>::Count() const
{
    return size;
}

template <typename T>
inline uint32_t
Array<T>::Capacity() const
{
    return capacity;
}

template <typename T>
inline bool
Array<T>::IsEmpty() const
{
    return 0 == size;
}

template <typename T>
inline const T*
Array<T>::Begin() const
{
    return data;
}

template <typename T>
inline T*
Array<T>::Begin()
{
    return data;
}

template <typename T>
inline const T*
Array<T>::End() const
{
    return data + size;
}

template <typename T>
T* Array<T>::End()
{
    return data + size;
}

template <typename T>
inline const T&
Array<T>::Front() const
{
    assert(size > 0);
    return *data;
}

template <typename T>
inline T&
Array<T>::Front()
{
    assert(size > 0);
    return *data;
}

template <typename T>
inline const T&
Array<T>::Back() const
{
    assert(size > 0);
    return *(data + size - 1);
}

template <typename T>
inline T&
Array<T>::Back()
{
    assert(size > 0);
    return *(data + size - 1);
}

template <typename T>
inline void
Array<T>::Clear()
{
    if (!std::is_trivially_copyable<T>::value && std::is_destructible<T>::value) {
        for (uint32_t i = 0; i < size; ++i)
            (data + i)->~T();
    }
    
    size = 0;
}

template <typename T>
inline void
Array<T>::Resize(uint32_t newSize)
{
    if (capacity < newSize)
        this->Grow(newSize);
    
    if (!std::is_trivially_copyable<T>::value) {
        if (std::is_default_constructible<T>::value && newSize > size) {
            for (uint32_t i = size; i < newSize; ++i)
                new(data + i) T();
        }
        
        if (std::is_destructible<T>::value && newSize < size) {
            for (uint32_t i = newSize; i < size; ++i)
                (data + i)->~T();
        }
    }
    
    size = newSize;
}

template <typename T>
inline void
Array<T>::SetCapacity(uint32_t newCapacity)
{
    if (capacity == newCapacity)
        return;
    
    if (newCapacity < size) {
        this->Resize(newCapacity);
        return;
    }
    
    T *newData = nullptr;
    if (newCapacity > 0) {
        newData = static_cast<T*>(allocator->Allocate(newCapacity * sizeof(T), __alignof(T)));
        Memory::Move(newData, data, size);
    }
    
    allocator->Free(data);
    
    data = newData;
    capacity = newCapacity;
}

template <typename T>
inline void
Array<T>::Reserve(uint32_t newCapacity)
{
    if (capacity < newCapacity)
        this->SetCapacity(newCapacity);
}

template <typename T>
inline void
Array<T>::PushBack(const T &item)
{
    if (size == capacity)
        this->Grow();
    Memory::Copy(data + (size++), &item, 1);
}

template <typename T>
inline void
Array<T>::Insert(uint32_t index, const T &item)
{
    assert(index <= size);
    if (index == size) {
        this->PushBack(item);
    } else {
        if (size == capacity)
            this->Grow();
        Memory::MoveReverse(data + index + 1, data + index, size - index);
        Memory::Copy(data + index, &item, 1);
        size++;
    }
}

template <typename T>
inline void
Array<T>::InsertRange(uint32_t index, const T *items, uint32_t count)
{
    assert(index <= size);
    if (size + count > capacity)
        this->Grow(size + count);
    if (index < size)
        Memory::MoveReverse(data + index + count, data + index, size - index);
    Memory::Copy(data + index, items, count);
    size += count;
}

template <typename T>
inline void
Array<T>::PopBack()
{
    assert(size > 0);
    size--;
    if (!std::is_trivially_copyable<T>::value && std::is_destructible<T>::value)
        (data + size)->~T();
}

template <typename T>
inline void
Array<T>::Remove(const T &item)
{
    for (uint32_t i = 0; i < size; ++i) {
        if (*(data + i) == item) {
            this->RemoveAt(i);
            break;
        }
    }
}

template <typename T>
inline void
Array<T>::RemoveAt(uint32_t index)
{
    assert(index < size);
    if (!std::is_trivially_copyable<T>::value && std::is_destructible<T>::value) {
        (data + index)->~T();
    }
    --size;
    if (size > 0 && index < size) {
        //Memory::Move(data + index, data + size, 1);
        Memory::Move(data + index, data + index + 1, size - index);
    }
}

template <typename T>
inline void
Array<T>::RemoveRange(uint32_t index, uint32_t count)
{
    uint32_t endIndex = index + count;
    assert(index < size && endIndex <= size);
    if (!std::is_trivially_copyable<T>::value && std::is_destructible<T>::value) {
        for (uint32_t i = index; i < endIndex; ++i)
            (data + i)->~T();
    }
    if (size > count && endIndex < size)
        Memory::Move(data + index, data + endIndex, size - endIndex);
    size -= count;
}

template <typename T>
inline void
Array<T>::Trim()
{
    if (0 == size) {
        if (capacity > 0) {
            allocator->Free(data);
            capacity = 0;
            data = nullptr;
        }
    } else {
        T *newData = static_cast<T*>(allocator->Allocate(sizeof(T) * size, __alignof(T)));
        Memory::Move(newData, data, size);
        allocator->Free(data);
        capacity = size;
        data = newData;
    }
}

template <typename T>
inline int32_t
Array<T>::IndexOf(const T &item)
{
    for (uint32_t i = 0; i < size; ++i) {
        if (*(data + i) == item)
            return i;
    }
    return -1;
}

template <typename T>
inline void
Array<T>::Sort(Array<T> &array, uint32_t index, uint32_t count, std::function<bool(const T&, const T&)> f)
{
    uint32_t right = index + count;
    assert(index < array.size && right <= array.size);
    std::sort(array.data + index, array.data + right, f);
}

template <typename T>
inline int32_t
BinarySearch(const Array<T> &array, uint32_t index, uint32_t count, const T &item)
{
    int32_t left = index, right = index + count - 1;
    assert(left <= array.Count() && right < (int32_t)array.Count());
    while (left <= right) {
        uint32_t pivot = (left + right) >> 1;
        const T& pivotItem = array[pivot];

        if (pivotItem < item)
            left = pivot + 1;
        else if (pivotItem > item)
            right = pivot - 1;
        else
            return pivot; // item found
    }
    return -(left + 1); // item not found
}
/*
template <typename T>
inline int32_t
BinarySearch(const Array<T> &array, uint32_t index, uint32_t count, const T &item, std::function<int(const T&, const T&)> f)
{
    int32_t left = index, right = index + count - 1;
    assert(left < array.size && right < array.size);
    while (left <= right) {
        uint32_t pivot = (left + right) >> 1;
        int cmp = f(*(array.data + pivot), item);

        if (cmp < 0)
            left = pivot + 1;
        else if (cmp > 0)
            right = pivot - 1;
        else
            return pivot; // item found
    }
    return -(left + 1); // item not found
}
*/
template <typename T, typename U>
inline int32_t
BinarySearch(const Array<T> &array, uint32_t index, uint32_t count, const U &item, std::function<int(const T&, const U&)> f)
{
    int32_t left = index, right = index + count - 1;
	assert(left <= (int32_t)array.Count() && right < (int32_t)array.Count());
    while (left <= right) {
        uint32_t pivot = (left + right) >> 1;
        int cmp = f(array[pivot], item);

        if (cmp < 0)
            left = pivot + 1;
        else if (cmp > 0)
            right = pivot - 1;
        else
            return pivot; // item found
    }
    return -(left + 1); // item not found
}

} // namespace Framework
