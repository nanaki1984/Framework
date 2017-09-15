#pragma once

#include "Core/Collections/Array_type.h"

namespace Framework {

struct HashEntry {
	uint32_t key;
	uint32_t index;
	uint32_t prev;
	uint32_t next;
};

template <typename T>
class Hash {
private:
    static const uint32_t kEndOfList = 0xffffffff;

    Array<uint32_t> map;
	Array<HashEntry> entries;
    Array<T> data;

    bool IsFull() const;
    void Grow();

	const HashEntry* FindFirst(uint32_t key) const;
	HashEntry* FindFirst(uint32_t key);
	const HashEntry* FindNext(const HashEntry *entry) const;
	HashEntry* FindNext(HashEntry *entry);

	HashEntry* Insert(uint32_t key, const T& item);
	void Erase(HashEntry *entry);
public:
    Hash(Allocator &allocator);
    Hash(Allocator &allocator, uint32_t initialMapSize);
    Hash(const Hash<T> &other);
    Hash(Hash<T> &&other);

    Hash<T>& operator =(const Hash<T> &other);
    Hash<T>& operator =(Hash<T> &&other);

    Hash<T> Rehash(uint32_t newMapSize);

    Allocator& GetAllocator() const;
    uint32_t Count() const;
    uint32_t Count(uint32_t key) const;
    bool IsEmpty() const;

    const T* Begin() const;
    T* Begin();
    const T* End() const;
    T* End();

    T* Get(uint32_t key);
    const T* Get(uint32_t key) const;
    Array<T*> GetAll(uint32_t key, Allocator &allocator);

    T* Find(uint32_t key, std::function<bool(const T&)> f);
    Array<T*> FindAll(uint32_t key, Allocator &allocator, std::function<bool(const T&)> f);

    T* Next(T *item);
    const T* Next(const T *item) const;

    void Clear();
    void Add(uint32_t key, const T& item);
    void Set(uint32_t key, const T& item);

    void Remove(T *item);
    void RemoveIf(uint32_t key, std::function<bool(const T&)> f);
    void RemoveAll(uint32_t key);
};

} // namespace Framework
