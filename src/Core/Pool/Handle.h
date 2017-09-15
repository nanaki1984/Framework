#pragma once

#include "Core/Pool/Handle_type.h"
#include "Core/Pool/BasePool.h"

template <class T>
Handle<T>::Handle()
: pool(nullptr),
  instanceId(0)
{ }

template <class T>
Handle<T>::Handle(T *p)
: pool(nullptr == p ? nullptr : p->GetPool()),
  instanceId(nullptr == p ? 0 : p->GetInstanceID())
{ }

template <class T>
inline void
Handle<T>::Set(T *p)
{
    if (nullptr == p) {
        pool = nullptr;
        instanceId = 0;
    } else {
        pool = p->GetPool();
        instanceId = p->GetInstanceID();
    }
}

template <class T>
inline T*
Handle<T>::Get() const
{
    if (nullptr == pool)
        return nullptr;
    else
        return static_cast<T*>(pool->Get(instanceId));
}

template <class T>
inline bool
Handle<T>::IsValid() const
{
    return (pool != nullptr && pool->Get(instanceId) != nullptr);
}

template <class T>
inline bool
Handle<T>::EqualsTo(const T *p) const
{
    if (nullptr == p)
        return !this->IsValid();
    else
        return p->GetPool() == pool && p->GetInstanceID() == instanceId;
}

template <class T>
inline Handle<T>&
Handle<T>::operator =(T *p)
{
    this->Set(p);
    return *this;
}

template <class T>
inline bool
Handle<T>::operator ==(const Handle<T> &p) const
{
    return (pool == p.pool && instanceId == p.instanceId);
}

template <class T>
inline bool
Handle<T>::operator !=(const Handle<T> &p) const
{
    return (pool != p.pool || instanceId != p.instanceId);
}

template <class T>
inline
Handle<T>::operator T*() const
{
    return nullptr == pool ? nullptr : static_cast<T*>(pool->Get(instanceId));
}

template <class T>
inline T*
Handle<T>::operator ->() const
{
    assert(this->IsValid());
    return static_cast<T*>(pool->Get(instanceId));
}

template <class T>
inline T&
Handle<T>::operator *() const
{
    assert(this->IsValid());
    return *static_cast<T*>(pool->Get(instanceId));
}

template <class T>
template <class U>
inline U*
Handle<T>::Cast() const
{
    assert(this->IsValid());
    T *ptr = this->operator T*();
    assert(ptr->template IsA<U>());
    return static_cast<U*>(ptr);
}

template <class T>
const Handle<T> Handle<T>::Null = nullptr;
