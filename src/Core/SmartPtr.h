#pragma once

#include "Core/Debug.h"
#include "Core/RefCounted.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"

template <class T>
class SmartPtr {
private:
    T *ptr;
public:
    SmartPtr();
    SmartPtr(const SmartPtr<T> &p);
    SmartPtr(SmartPtr<T> &&p);
    ~SmartPtr();

    T* Get() const;
    void Reset();
    bool IsValid() const;

    SmartPtr<T>& operator = (const SmartPtr<T> &p);
    SmartPtr<T>& operator = (SmartPtr<T> &&p);

    bool operator == (const SmartPtr<T> &p) const;
    bool operator != (const SmartPtr<T> &p) const;
    bool operator == (const T *p) const;
    bool operator != (const T *p) const;

    operator T* () const;
    T* operator -> () const;
    T& operator * () const;

    template <typename A>
    static SmartPtr<T> MakeNew();

    template <typename A, typename U>
    static SmartPtr<T> MakeNew();

    template <typename A, typename ...Args>
    static SmartPtr<T> MakeNew(Args... params);

    template <typename A, typename U, typename ...Args>
    static SmartPtr<T> MakeNew(Args... params);

    template <class U>
    static SmartPtr<T> CastFrom(const SmartPtr<U>& p);

    friend class Framework::ClassInfo;
};

template <class T>
SmartPtr<T>::SmartPtr()
: ptr(nullptr)
{ }

template <class T>
SmartPtr<T>::SmartPtr(const SmartPtr<T> &p)
: ptr(p.ptr)
{
    if (nullptr != ptr) ptr->AddRef();
}

template <class T>
SmartPtr<T>::SmartPtr(SmartPtr<T> &&p)
: ptr(p.ptr)
{
    p.ptr = nullptr;
}

template <class T>
SmartPtr<T>::~SmartPtr( )
{
    if (nullptr != ptr) ptr->Release();
}

template <class T>
inline T*
SmartPtr<T>::Get() const
{
    return ptr;
}

template <class T>
inline void
SmartPtr<T>::Reset()
{
    if (nullptr != ptr) ptr->Release();
    ptr = nullptr;
}

template <class T>
inline bool
SmartPtr<T>::IsValid() const
{
    return (nullptr != ptr);
}

template <class T>
inline SmartPtr<T>&
SmartPtr<T>::operator = (const SmartPtr<T> &p)
{
    if (nullptr != ptr) ptr->Release();
    ptr = p;
    if (nullptr != ptr) ptr->AddRef();
    return *this;
}

template <class T>
inline SmartPtr<T>&
SmartPtr<T>::operator = (SmartPtr<T> &&p)
{
    if (nullptr != ptr) ptr->Release();
    ptr = p.ptr;
    p.ptr = nullptr;
    return *this;
}

template <class T>
inline bool
SmartPtr<T>::operator == (const SmartPtr<T> &p) const
{
    return (ptr == p.ptr);
}

template <class T>
inline bool
SmartPtr<T>::operator != (const SmartPtr<T> &p) const
{
    return (ptr != p.ptr);
}

template <class T>
inline bool
SmartPtr<T>::operator == (const T *p) const
{
    return (ptr == p);
}

template <class T>
inline bool
SmartPtr<T>::operator != (const T *p) const
{
    return (ptr != p);
}

template <class T>
inline
SmartPtr<T>::operator T* () const
{
    assert(ptr);
    return ptr;
}

template <class T>
inline T*
SmartPtr<T>::operator -> () const
{
    assert(ptr);
    return ptr;
}

template <class T>
inline T&
SmartPtr<T>::operator * () const
{
    assert(ptr);
    return *ptr;
}

template <class T>
template <typename A>
SmartPtr<T>
SmartPtr<T>::MakeNew()
{
    A *a = &Framework::Memory::GetAllocator<A>();
    Framework::RefCounted *p = new(a->Allocate(sizeof(T), __alignof(T))) T();
    p->allocator = a;

    SmartPtr<T> sp;
    sp.ptr = static_cast<T*>(p);
    sp.ptr->AddRef();

    return sp;
}

template <class T>
template <typename A, typename U>
SmartPtr<T>
SmartPtr<T>::MakeNew()
{
    A *a = &Framework::Memory::GetAllocator<A>();
    Framework::RefCounted *p = new(a->Allocate(sizeof(U), __alignof(U))) U();
    p->allocator = a;

    SmartPtr<T> sp;
    sp.ptr = static_cast<T*>(static_cast<U*>(p));
    sp.ptr->AddRef();

    return sp;
}

template <class T>
template <typename A, typename ...Args>
SmartPtr<T>
SmartPtr<T>::MakeNew(Args... params)
{
    A *a = &Framework::Memory::GetAllocator<A>();
    Framework::RefCounted *p = new(a->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args...>(params...));
    p->allocator = a;

    SmartPtr<T> sp;
    sp.ptr = static_cast<T*>(p);
    sp.ptr->AddRef();

    return sp;
}

template <class T>
template <typename A, typename U, typename ...Args>
SmartPtr<T>
SmartPtr<T>::MakeNew(Args... params)
{
    A *a = &Framework::Memory::GetAllocator<A>();
    Framework::RefCounted *p = new(a->Allocate(sizeof(U), __alignof(U))) U(std::forward<Args...>(params...));
    p->allocator = a;

    SmartPtr<T> sp;
    sp.ptr = static_cast<T*>(static_cast<U*>(p));
    sp.ptr->AddRef();

    return sp;
}

template <class T>
template <class U>
SmartPtr<T>
SmartPtr<T>::CastFrom(const SmartPtr<U>& p)
{
    SmartPtr<T> sp;
    sp.ptr = static_cast<T*>(p.Get());
    sp.ptr->AddRef();
    return sp;
}
