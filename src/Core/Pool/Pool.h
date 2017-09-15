#pragma once

#include "Core/Pool/Pool_type.h"
#include "Core/Pool/Handle.h"

namespace Framework {

template <typename T>
inline
Pool<T>::Pool(Allocator &allocator)
: BasePool(allocator, &T::RTTI)
{ }

template <typename T>
inline
Pool<T>::Pool(const Pool<T> &other)
: BasePool(other)
{
    this->MoveObjects(reinterpret_cast<void*>(data), reinterpret_cast<void*>(other.data), other.size);

    uintptr_t obj = uintptr_t(data);
    for (uint32_t i = 0; i < size; ++i, obj += classInfo->GetSize())
        reinterpret_cast<BaseObject*>(obj)->pool = this;
}

template <typename T>
inline
Pool<T>::Pool(Pool<T> &&other)
: BasePool(std::forward<BasePool>(other))
{ }

template <typename T>
inline
Pool<T>::~Pool()
{
    this->Clear();
}

template <typename T>
inline void
Pool<T>::CopyObjects(void *dest, void *src, uint32_t objectsCount)
{
    T *ptr0 = static_cast<T*>(dest), *ptr1 = static_cast<T*>(src);
    for (uint32_t i = 0; i < objectsCount; ++i, ++ptr0, ++ptr1)
        new(ptr0) T(*ptr1);
}

template <typename T>
inline void
Pool<T>::MoveObjects(void *dest, void *src, uint32_t objectsCount)
{
    T *ptr0 = static_cast<T*>(dest), *ptr1 = static_cast<T*>(src);
    for (uint32_t i = 0; i < objectsCount; ++i, ++ptr0, ++ptr1)
        new(ptr0) T(std::forward<T>(*ptr1));
}

template <typename T>
inline T*
Pool<T>::Begin()
{
    return static_cast<T*>(data);
}

template <typename T>
inline const T*
Pool<T>::Begin() const
{
    return static_cast<const T*>(data);
}

template <typename T>
inline T*
Pool<T>::End()
{
    return reinterpret_cast<T*>(uintptr_t(data) + classInfo->GetSize() * size);
}

template <typename T>
inline const T*
Pool<T>::End() const
{
    return reinterpret_cast<const T*>(uintptr_t(data) + classInfo->GetSize() * size);
}

template <typename T>
inline Handle<T>
Pool<T>::NewInstance()
{
    T *ptr = static_cast<T*>(this->Allocate());
    new(ptr) T();
    return ptr;
}

template <typename T>
template <typename... Args>
inline Handle<T>
Pool<T>::NewInstance(Args... arguments)
{
    T *ptr = static_cast<T*>(this->Allocate());
    //new(ptr) T(std::forward<Args...>(arguments...));
    new(ptr) T(arguments...);
    return ptr;
}

template <typename T>
inline Handle<T>
Pool<T>::GetInstance(uint32_t id)
{
    return static_cast<T*>(this->Get(id));
}

template <typename T>
inline Handle<T>
Pool<T>::CloneInstance(const Handle<T> &handle)
{
    return static_cast<T*>(this->Clone(handle->GetInstanceID()));
}

template <typename T>
inline void
Pool<T>::DeleteInstance(const Handle<T> &handle)
{
    this->Free(handle.Get());
}

} // namespace Framework
