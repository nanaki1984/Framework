#include "Core/Pool/BasePool.h"
#include "Core/Collections/Array.h"
#include "Core/Pool/BaseObject.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"

namespace Framework {

BasePool::BasePool(Allocator &allocator, ClassInfo *_classInfo)
: nextId(0),
  classInfo(_classInfo),
  map(allocator),
  entries(allocator),
  data(nullptr),
  size(0),
  capacity(0)
{ }

BasePool::BasePool(const BasePool &other)
: nextId(other.nextId),
  classInfo(other.classInfo),
  map(other.map),
  entries(other.entries),
  data(nullptr),
  size(other.size),
  capacity(other.capacity)
{
    data = this->GetAllocator().Allocate(classInfo->GetSize() * capacity, classInfo->GetAlign());
}

BasePool::BasePool(BasePool &&other)
: nextId(other.nextId),
  classInfo(other.classInfo),
  map(std::forward<Array<uint32_t>>(other.map)),
  entries(std::forward<Array<Entry>>(other.entries)),
  data(other.data),
  size(other.size),
  capacity(other.capacity)
{
    other.nextId = 0;
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;

    uintptr_t obj = uintptr_t(data);
    for (uint32_t i = 0; i < size; ++i, obj += classInfo->GetSize())
        reinterpret_cast<BaseObject*>(obj)->pool = this;
}

BasePool::~BasePool()
{
    this->GetAllocator().Free(data);
}

BasePool&
BasePool::operator =(const BasePool &other)
{
    nextId = other.nextId;
    classInfo = other.classInfo;
    map = other.map;
    entries = other.entries;
    size = other.size;
    capacity = other.capacity;

    data = this->GetAllocator().Allocate(classInfo->GetSize() * capacity, classInfo->GetAlign());

    this->CopyObjects(data, other.data, other.size);

    uintptr_t obj = uintptr_t(data);
    for (uint32_t i = 0; i < size; ++i, obj += classInfo->GetSize())
        reinterpret_cast<BaseObject*>(obj)->pool = this;

    return (*this);
}

BasePool&
BasePool::operator =(BasePool &&other)
{
    nextId = other.nextId;
    classInfo = other.classInfo;
    map = std::forward<Array<uint32_t>>(other.map);
    entries = std::forward<Array<Entry>>(other.entries);
    data = other.data;
    size = other.size;
    capacity = other.capacity;

    other.nextId = 0;
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;

    uintptr_t obj = uintptr_t(data);
    for (uint32_t i = 0; i < size; ++i, obj += classInfo->GetSize())
        reinterpret_cast<BaseObject*>(obj)->pool = this;

    return (*this);
}

void
BasePool::Grow()
{
    capacity = 8 + capacity * 2;

    void *newData = this->GetAllocator().Allocate(classInfo->GetSize() * capacity, classInfo->GetAlign());
    this->MoveObjects(newData, data, size);

    this->GetAllocator().Free(data);
    data = newData;
}

bool
BasePool::IsHashFull() const
{
    const float maxLoadFactor = 0.7f;
    return (0 == map.Count() || entries.Count() >= (map.Count() * maxLoadFactor));
}

void
BasePool::Rehash(uint32_t newMapSize)
{
    map.Resize(newMapSize);
    uint32_t i = 0;
    for (; i < newMapSize; ++i)
        map[i] = kEndOfList;

    entries.Clear();

    uintptr_t obj = uintptr_t(data);
    for (i = 0; i < size; ++i, obj += classInfo->GetSize())
        this->Insert(reinterpret_cast<BaseObject*>(obj));
}

void
BasePool::Insert(BaseObject *pointer)
{
    assert(nullptr == this->Get(pointer->id));

    if (this->IsHashFull())
        this->Rehash(map.Count() * 2 + 8);

    entries.PushBack(Entry());
    Entry &entry = entries.Back();

    uint32_t entryIndex = &entry - entries.Begin(),
             entryHash  = pointer->id % map.Count();

    entry.id    = pointer->id;
    entry.index = entryIndex;
    entry.prev  = kEndOfList;
    entry.next  = kEndOfList;

    uint32_t prevIndex = map[entryHash];
    map[entryHash] = entryIndex;

    Entry *firstEntry = kEndOfList == prevIndex ? nullptr : entries.Begin() + prevIndex;
    if (firstEntry != nullptr) {
        entry.next = firstEntry->index;
        firstEntry->prev = entryIndex;
    }
}

void
BasePool::Remove(BaseObject *pointer)
{
    uint32_t objIndex = (uintptr_t(pointer) - uintptr_t(data)) / classInfo->GetSize();
    assert(objIndex < size);

    Entry *entry = entries.Begin() + objIndex;
    assert(entry->index == objIndex);

    if (kEndOfList == entry->prev)
        map[entry->id % map.Count()] = entry->next;
    else
        entries[entry->prev].next = entry->next;

    if (entry->next != kEndOfList)
        entries[entry->next].prev = entry->prev;

    int index     = entry->index,
        lastIndex = entries.Count() - 1;

    if (index == lastIndex) {
        entries.PopBack();
        return;
    }

    entries[index] = entries[lastIndex];
    entries.PopBack();

    Entry *movedEntry = entries.Begin() + index;
    movedEntry->index = index;
    if (kEndOfList == movedEntry->prev)
        map[movedEntry->id % map.Count()] = index;
    else
        entries[movedEntry->prev].next = index;

    if (movedEntry->next != kEndOfList)
        entries[movedEntry->next].prev = index;
}

BaseObject*
BasePool::Allocate()
{
    if (size == capacity)
        this->Grow();

    BaseObject *pointer = reinterpret_cast<BaseObject*>(uintptr_t(data) + classInfo->GetSize() * size);

    pointer->pool = this;
    pointer->id   = nextId++;

    this->Insert(pointer);

    ++size;

    return pointer;
}

void
BasePool::Free(BaseObject *pointer)
{
    pointer->~BaseObject();
    pointer->pool = nullptr;

    this->Remove(pointer);

    --size;
    if (size > 0) {
        void *lastPointer = reinterpret_cast<void*>(uintptr_t(data) + classInfo->GetSize() * size);
        this->MoveObjects(pointer, lastPointer, 1);
    }
}

const ClassInfo*
BasePool::GetBaseObjectClassInfo() const
{
    return classInfo;
}

Allocator&
BasePool::GetAllocator() const
{
    return map.GetAllocator();
}

uint32_t
BasePool::Count() const
{
    return size;
}

uint32_t
BasePool::Capacity() const
{
    return capacity;
}

bool
BasePool::IsEmpty() const
{
    return 0 == size;
}

void
BasePool::Clear()
{
    int32_t i = size - 1;
    uintptr_t obj = uintptr_t(data) + classInfo->GetSize() * i;
    for (; i >= 0; --i, obj -= classInfo->GetSize())
        reinterpret_cast<BaseObject*>(obj)->Destroy();

    assert(0 == size);
}

const BaseObject*
BasePool::Get(uint32_t id) const
{
    uint32_t mapIndex = map[id % map.Count()];
    if (kEndOfList == mapIndex)
        return nullptr;
    else {
        const Entry *entry = entries.Begin() + mapIndex;
        while (entry->id != id) {
            if (kEndOfList == entry->next)
                return nullptr;
            entry = entries.Begin() + entry->next;
        }
        return reinterpret_cast<const BaseObject*>(uintptr_t(data) + classInfo->GetSize() * entry->index);
    }
}

BaseObject*
BasePool::Get(uint32_t id)
{
    if (0 == size)
        return nullptr;

    uint32_t mapIndex = map[id % map.Count()];
    if (kEndOfList == mapIndex)
        return nullptr;
    else {
        Entry *entry = entries.Begin() + mapIndex;
        while (entry->id != id) {
            if (kEndOfList == entry->next)
                return nullptr;
            entry = entries.Begin() + entry->next;
        }
        return reinterpret_cast<BaseObject*>(uintptr_t(data) + classInfo->GetSize() * entry->index);
    }
}

BaseObject*
BasePool::Clone(uint32_t id)
{
    BaseObject *dst = this->Allocate();
    BaseObject *src = this->Get(id);
    assert(src != nullptr);
    this->CopyObjects(dst, src, 1);
    return dst;
}

} // namespace Framework
