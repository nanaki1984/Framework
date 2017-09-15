#pragma once

#include "Core/Collections/Hash_type.h"
#include "Core/Collections/Array.h"

namespace Framework {

template <typename T>
inline
Hash<T>::Hash(Allocator &allocator)
: map(allocator),
  entries(allocator),
  data(allocator)
{ }

template <typename T>
inline
Hash<T>::Hash(Allocator &allocator, uint32_t initialMapSize)
: map(allocator, initialMapSize),
  entries(allocator),
  data(allocator)
{
    map.Resize(initialMapSize);
    for (uint32_t i = 0; i < initialMapSize; ++i)
        map[i] = kEndOfList;
}

template <typename T>
inline
Hash<T>::Hash(const Hash<T> &other)
: map(other.map),
  entries(other.entries),
  data(other.data)
{ }

template <typename T>
inline
Hash<T>::Hash(Hash<T> &&other)
: map(std::forward<Array<uint32_t>>(other.map)),
  entries(std::forward<Array<HashEntry>>(other.entries)),
  data(std::forward<Array<T>>(other.data))
{ }

template <typename T>
inline Hash<T>&
Hash<T>::operator =(const Hash<T> &other)
{
    map = other.map;
    entries = other.entries;
    data = other.data;
    return (*this);
}

template <typename T>
inline Hash<T>&
Hash<T>::operator =(Hash<T> &&other)
{
    map = std::forward<Array<uint32_t>>(other.map);
	entries = std::forward<Array<HashEntry>>(other.entries);
	data = std::forward<Array<T>>(other.data);
    return (*this);
}

template <typename T>
inline bool
Hash<T>::IsFull() const
{
    const float maxLoadFactor = 0.7f;
    return (0 == map.Count() || entries.Count() >= (map.Count() * maxLoadFactor));
}

template <typename T>
inline void
Hash<T>::Grow()
{
    (*this) = this->Rehash(map.Count() * 2 + 8);
}

template <typename T>
inline Hash<T>
Hash<T>::Rehash(uint32_t newMapSize)
{
    Hash<T> h(map.GetAllocator(), newMapSize);
	HashEntry *entry = entries.Begin(), *end = entries.End();
    for (; entry < end; ++entry)
        h.Insert(entry->key, data[entry->index]);
    return h;
}

template <typename T>
inline const HashEntry*
Hash<T>::FindFirst(uint32_t key) const
{
    if (map.IsEmpty())
        return nullptr;

    uint32_t hash  = key % map.Count(),
             index = map[hash];

    if (kEndOfList == index)
        return nullptr;
    else {
        while (entries[index].key != key) {
            index = entries[index].next;

            if (kEndOfList == index)
                return nullptr;
        }

        return entries.Begin() + index;
    }
}

template <typename T>
inline HashEntry*
Hash<T>::FindFirst(uint32_t key)
{
    if (map.IsEmpty())
        return nullptr;

    uint32_t hash  = key % map.Count(),
             index = map[hash];

    if (kEndOfList == index)
        return nullptr;
    else {
        while (entries[index].key != key) {
            index = entries[index].next;
            
            if (kEndOfList == index)
                return nullptr;
        }

        return entries.Begin() + index;
    }
}

template <typename T>
inline const HashEntry*
Hash<T>::FindNext(const HashEntry *entry) const
{
    uint32_t next = entry->next;
    if (kEndOfList == next || entries[next].key != entry->key)
        return nullptr;
    return entries.Begin() + next;
}

template <typename T>
inline HashEntry*
Hash<T>::FindNext(HashEntry *entry)
{
    uint32_t next = entry->next;
    if (kEndOfList == next || entries[next].key != entry->key)
        return nullptr;
    return entries.Begin() + next;
}

template <typename T>
inline HashEntry*
Hash<T>::Insert(uint32_t key, const T& item)
{
    if (this->IsFull())
        this->Grow();

	entries.PushBack(HashEntry());
    data.PushBack(item);

	HashEntry &entry = entries.Back();

    uint32_t entryIndex = &entry - entries.Begin(),
             entryHash  = key % map.Count();

    entry.key   = key;
    entry.index = entryIndex;
    entry.prev  = kEndOfList;
    entry.next  = kEndOfList;

    HashEntry *firstEntry = this->FindFirst(key);
    if (nullptr == firstEntry)
        firstEntry = (kEndOfList == map[entryHash] ? nullptr : entries.Begin() + map[entryHash]);

    if (nullptr == firstEntry) {
        map[entryHash] = entryIndex;
    } else {
        entry.prev = firstEntry->prev;
        if (kEndOfList == entry.prev)
            map[entryHash] = entryIndex;
        else
            entries[entry.prev].next = entryIndex;

        entry.next = firstEntry->index;
        firstEntry->prev = entryIndex;
    }

    return &entry;
}

template <typename T>
inline void
Hash<T>::Erase(HashEntry *entry)
{
    if (kEndOfList == entry->prev)
        map[entry->key % map.Count()] = entry->next;
    else
        entries[entry->prev].next = entry->next;

    if (entry->next != kEndOfList)
        entries[entry->next].prev = entry->prev;

    int index     = entry->index,
        lastIndex = entries.Count() - 1;

    if (index == lastIndex) {
        entries.PopBack();
        data.PopBack();
        return;
    }

    entries[index] = entries[lastIndex];
    Memory::Move(data.Begin() + index, data.Begin() + lastIndex, 1);

    entries.PopBack();
    data.PopBack();

	HashEntry *movedEntry = entries.Begin() + index;
    movedEntry->index = index;
    if (kEndOfList == movedEntry->prev)
        map[movedEntry->key % map.Count()] = index;
    else
        entries[movedEntry->prev].next = index;
    if (movedEntry->next != kEndOfList)
        entries[movedEntry->next].prev = index;
}

template <typename T>
Allocator&
Hash<T>::GetAllocator() const
{
    return map.GetAllocator();
}

template <typename T>
inline uint32_t
Hash<T>::Count() const
{
    return entries.Count();
}

template <typename T>
inline uint32_t
Hash<T>::Count(uint32_t key) const
{
	HashEntry *entry = this->FindFirst(key);
    uint32_t counter = 0;
    while (entry != nullptr) {
        ++counter;
        entry = this->FindNext(entry);
    }
    return counter;
}

template <typename T>
inline bool
Hash<T>::IsEmpty() const
{
    return entries.IsEmpty();
}

template <typename T>
inline const T*
Hash<T>::Begin() const
{
    return data.Begin();
}

template <typename T>
inline T*
Hash<T>::Begin()
{
    return data.Begin();
}

template <typename T>
inline const T*
Hash<T>::End() const
{
    return data.End();
}

template <typename T>
inline T*
Hash<T>::End()
{
    return data.End();
}

template <typename T>
inline T*
Hash<T>::Get(uint32_t key)
{
	const HashEntry *entry = this->FindFirst(key);
    if (nullptr == entry)
        return nullptr;
    else
        return data.Begin() + entry->index;
}

template <typename T>
inline const T*
Hash<T>::Get(uint32_t key) const
{
	const HashEntry *entry = this->FindFirst(key);
    if (nullptr == entry)
        return nullptr;
    else
        return data.Begin() + entry->index;
}

template <typename T>
inline Array<T*>
Hash<T>::GetAll(uint32_t key, Allocator &allocator)
{
    Array<T*> array(allocator);
	HashEntry *entry = this->FindFirst(key);
    while (entry != nullptr) {
        array.PushBack(data.Begin() + entry->index);
        entry = this->FindNext(entry);
    }
    return array;
}

template <typename T>
inline T*
Hash<T>::Find(uint32_t key, std::function<bool(const T&)> f)
{
	HashEntry *entry = this->FindFirst(key);
    while (entry != nullptr) {
        if (f(data[entry->index]))
            return data.Begin() + entry->index;
        entry = this->FindNext(entry);
    }
    return nullptr;
}

template <typename T>
inline Array<T*>
Hash<T>::FindAll(uint32_t key, Allocator &allocator, std::function<bool(const T&)> f)
{
    Array<T*> array(allocator);
	HashEntry *entry = this->FindFirst(key);
    while (entry != nullptr) {
        if (f(data[entry->index]))
            array.PushBack(data.Begin() + entry->index);
        entry = this->FindNext(entry);
    }
    return array;
}

template <typename T>
inline T*
Hash<T>::Next(T *item)
{
    uint32_t index = item - data.Begin();
    assert(index < data.Count());
	HashEntry *entry = entries.Begin() + index;
    entry = this->FindNext(entry);
    if (nullptr == entry)
        return nullptr;
    else
        return data.Begin() + entry->index;
}

template <typename T>
inline const T*
Hash<T>::Next(const T *item) const
{
    uint32_t index = item - data.Begin();
    assert(index < data.Count());
	const HashEntry *entry = entries.Begin() + index;
    entry = this->FindNext(entry);
    if (nullptr == entry)
        return nullptr;
    else
        return data.Begin() + entry->index;
}

template <typename T>
inline void
Hash<T>::Clear()
{
    for (uint32_t i = 0, l = map.Count(); i < l; ++i)
        map[i] = kEndOfList;
    entries.Clear();
    data.Clear();
}

template <typename T>
inline void
Hash<T>::Add(uint32_t key, const T& item)
{
    this->Insert(key, item);
}

template <typename T>
inline void
Hash<T>::Set(uint32_t key, const T& item)
{
	HashEntry *entry = this->FindFirst(key);
    if (nullptr == entry)
        this->Insert(key, item);
    else
        data[entry->index] = item;
}

template <typename T>
inline void
Hash<T>::Remove(T *item)
{
    assert(item >= data.Begin() && item < data.End());
	HashEntry *entry = entries.Begin() + (item - data.Begin());
    this->Erase(entry);
}

template <typename T>
inline void
Hash<T>::RemoveIf(uint32_t key, std::function<bool(const T&)> f)
{
	const HashEntry *entry = this->FindFirst(key);
    while (entry != nullptr) {
		const HashEntry *tmp = entry;
        entry = this->FindNext(entry);
        if (f(data[tmp->index]))
            this->Erase(tmp);
    }
}

template <typename T>
inline void
Hash<T>::RemoveAll(uint32_t key)
{
	HashEntry *entry = this->FindFirst(key);
    while (entry != nullptr) {
		HashEntry *tmp = entry;
        entry = this->FindNext(entry);
        this->Erase(tmp);
    }
}

} // namespace Framework
