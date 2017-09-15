#pragma once

#include <cstdint>
#include <functional>

namespace Framework {

class Allocator;
        
template <typename T>
class Array
{
private:
    uint32_t size;
    uint32_t capacity;
    T        *data;

    Allocator *allocator;

    void Grow(uint32_t minCapacity = 0);
public:
    Array(Allocator &_allocator);
    Array(Allocator &_allocator, uint32_t initialCapacity);
    Array(const Array<T> &other);
    Array(Array<T> &&other);
    ~Array();

    Array<T>& operator =(const Array<T> &other);
    Array<T>& operator =(Array<T> &&other);

    const T& operator [](uint32_t index) const;
    T& operator [](uint32_t index);

    Allocator& GetAllocator() const;
    uint32_t Count() const;
    uint32_t Capacity() const;
    bool IsEmpty() const;

    const T* Begin() const;
    T* Begin();
    const T* End() const;
    T* End();

    const T& Front() const;
    T& Front();
    const T& Back() const;
    T& Back();

    void Clear();
    void Resize(uint32_t newSize);
    void SetCapacity(uint32_t newCapacity);
    void Reserve(uint32_t newCapacity);

    void PushBack(const T &item);
    void Insert(uint32_t index, const T &item);
    void InsertRange(uint32_t index, const T *items, uint32_t count);

    void PopBack();
    void Remove(const T &item);
    void RemoveAt(uint32_t index);
    void RemoveRange(uint32_t index, uint32_t count);

    void Trim();

    int32_t IndexOf(const T &item);

    static void Sort(Array<T> &array, uint32_t index, uint32_t count, std::function<bool(const T&, const T&)> f);
};

template <typename T>
static int32_t BinarySearch(const Array<T> &array, uint32_t index, uint32_t count, const T &item);
/*
template <typename T>
static int32_t BinarySearch(const Array<T> &array, uint32_t index, uint32_t count, const T &item, std::function<int(const T&, const T&)> f);
*/
template <typename T, typename U>
static int32_t BinarySearch(const Array<T> &array, uint32_t index, uint32_t count, const U &item, std::function<int(const T&, const U&)> f);

} // namespace Framework
