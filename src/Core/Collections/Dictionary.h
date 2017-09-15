#pragma once

#include "Core/Collections/Dictionary_type.h"
#include "Core/Collections/Array.h"
#include "Core/String.h"

namespace Framework {

template <typename K, typename V>
inline int32_t
BinarySearch(const Array<KeyValuePair<K, V>> &array, uint32_t index, uint32_t count, const K &item)
{
    int32_t left = index, right = index + count - 1;
	assert(uint32_t(left) < array.Count() && uint32_t(right) < array.Count());
    while (left <= right) {
        uint32_t pivot = (left + right) >> 1;
        const K &pivotItem = array[pivot].key;

        if (pivotItem < item)
            left = pivot + 1;
        else if (pivotItem > item)
            right = pivot - 1;
        else
            return pivot; // item found
    }
    return -(left + 1); // item not found
}

template <typename K, typename V>
inline
KeyValuePair<K, V>::KeyValuePair()
{ }

template <typename K, typename V>
inline
KeyValuePair<K, V>::KeyValuePair(const K &k)
: key(k)
{ }

template <typename K, typename V>
inline
KeyValuePair<K, V>::KeyValuePair(const K &k, const V &v)
: key(k),
  value(v)
{ }

template <typename K, typename V>
inline
KeyValuePair<K, V>::KeyValuePair(const KeyValuePair<K, V> &other)
: key(other.key),
  value(other.value)
{ }

template <typename K, typename V>
inline
KeyValuePair<K, V>::KeyValuePair(KeyValuePair<K, V> &&other)
: key(std::forward<K>(other.key)),
  value(std::forward<V>(other.value))
{ }

template <typename K, typename V>
inline
KeyValuePair<K, V>::~KeyValuePair()
{ }

template <typename K, typename V>
inline KeyValuePair<K, V>&
KeyValuePair<K, V>::operator =(const KeyValuePair<K, V> &other)
{
    key = other.key;
    value = other.value;
    return (*this);
}

template <typename K, typename V>
inline KeyValuePair<K, V>&
KeyValuePair<K, V>::operator =(KeyValuePair<K, V> &&other)
{
    key = std::forward<K>(other.key);
    value = std::forward<V>(other.value);
    return (*this);
}

template <typename K, typename V>
inline bool
KeyValuePair<K, V>::operator <(const KeyValuePair<K, V> &other) const
{
    return key < other.key;
}

template <typename K, typename V>
inline bool
KeyValuePair<K, V>::operator >(const KeyValuePair<K, V> &other) const
{
    return key > other.key;
}

template <typename K, typename V>
std::function<int(const KeyValuePair<K, V> &, const KeyValuePair<K, V> &)> Dictionary<K, V>::sortFunction =
[] (const KeyValuePair<K, V> &item1, const KeyValuePair<K, V> &item2) {
    if (item1.key > item2.key)
        return 1;
    else if (item1.key < item2.key)
        return -1;
    else
        return 0;
};

template <typename K, typename V>
inline
Dictionary<K, V>::Dictionary(Allocator &allocator)
: data(allocator),
  bulkAdding(false)
{ }

template <typename K, typename V>
inline
Dictionary<K, V>::Dictionary(Allocator &allocator, uint32_t initialCapacity)
: data(allocator, initialCapacity),
  bulkAdding(false)
{ }

template <typename K, typename V>
inline
Dictionary<K, V>::Dictionary(const Dictionary<K, V> &other)
: data(other.data),
  bulkAdding(false)
{ }

template <typename K, typename V>
inline
Dictionary<K, V>::Dictionary(Dictionary<K, V> &&other)
: data(std::forward(other.data)),
  bulkAdding(false)
{ }

template <typename K, typename V>
inline
Dictionary<K, V>::~Dictionary()
{ }

template <typename K, typename V>
inline Dictionary<K, V>&
Dictionary<K, V>::operator =(const Dictionary<K, V> &other)
{
    data = other.data;
    bulkAdding = other.bulkAdding;
    return (*this);
}

template <typename K, typename V>
inline Dictionary<K, V>&
Dictionary<K, V>::operator =(Dictionary<K, V> &&other)
{
    data = std::forward(other.data);
    bulkAdding = other.bulkAdding;
    return (*this);
}

template <typename K, typename V>
inline const V&
Dictionary<K, V>::operator [](const K &key) const
{
    assert(!bulkAdding);
    assert(!data.IsEmpty());
    int32_t index = BinarySearch(data, 0, data.Count(), key);
    assert(index >= 0);
    return data[index].value;
}

template <typename K, typename V>
inline V&
Dictionary<K, V>::operator [](const K &key)
{
    assert(!bulkAdding);
    if (data.IsEmpty()) {
        data.PushBack(KeyValuePair<K, V>(key, V()));
        return data[0].value;
    }
    int32_t index = BinarySearch(data, 0, data.Count(), key);
    if (index < 0) {
        index = ~index;
        data.Insert(index, KeyValuePair<K, V>(key, V()));
    }
    return data[index].value;
}

template <typename K, typename V>
inline Allocator&
Dictionary<K, V>::GetAllocator() const
{
    return data.GetAllocator();
}

template <typename K, typename V>
inline uint32_t
Dictionary<K, V>::Count() const
{
    return data.Count();
}

template <typename K, typename V>
inline bool
Dictionary<K, V>::IsEmpty() const
{
    return data.IsEmpty();
}

template <typename K, typename V>
inline const KeyValuePair<K, V>*
Dictionary<K, V>::Begin() const
{
    return data.Begin();
}

template <typename K, typename V>
inline KeyValuePair<K, V>*
Dictionary<K, V>::Begin()
{
    return data.Begin();
}

template <typename K, typename V>
inline const KeyValuePair<K, V>*
Dictionary<K, V>::End() const
{
    return data.End();
}

template <typename K, typename V>
inline KeyValuePair<K, V>*
Dictionary<K, V>::End()
{
    return data.End();
}

template <typename K, typename V>
inline bool
Dictionary<K, V>::Contains(const K &key) const
{
    assert(!bulkAdding);
    return BinarySearch(data, 0, data.Count(), key) >= 0;
}

template <typename K, typename V>
inline bool
Dictionary<K, V>::TryGetValue(const K &key, V &value) const
{
    assert(!bulkAdding);
    int32_t index = BinarySearch(data, 0, data.Count(), key);
    if (index >= 0)
        value = data[index].value;
}

template <typename K, typename V>
inline void
Dictionary<K, V>::Clear()
{
    data.Clear();
    bulkAdding = false;
}

template <typename K, typename V>
inline void
Dictionary<K, V>::Add(const K &key, const V &value)
{
    assert(!bulkAdding);
    if (data.IsEmpty()) {
        data.PushBack(KeyValuePair<K, V>(key, value));
        return;
    }
    int32_t index = BinarySearch(data, 0, data.Count(), key);
    assert2(index < 0, "Key already exist");
    index = ~index;
    data.Insert(index, KeyValuePair<K, V>(key, value));
}

template <typename K, typename V>
inline void
Dictionary<K, V>::Remove(const K &key)
{
    assert(!bulkAdding);
    int32_t index = BinarySearch(data, 0, data.Count(), key);
    if (index >= 0)
        data.RemoveAt(index);
}

template <typename K, typename V>
inline void
Dictionary<K, V>::BeginBulkAdd()
{
    assert(!bulkAdding);
    bulkAdding = true;
}

template <typename K, typename V>
inline void
Dictionary<K, V>::BulkAdd(const K &key, const V &value)
{
    assert(bulkAdding);
    data.PushBack(KeyValuePair<K, V>(key, value));
}

template <typename K, typename V>
inline void
Dictionary<K, V>::EndBulkAdd()
{
    assert(bulkAdding);
    bulkAdding = false;
    Array<KeyValuePair<K, V>>::Sort(data, 0, data.Count(), sortFunction);
}

} // namespace Framework
