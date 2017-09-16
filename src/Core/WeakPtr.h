#pragma once

#include "Core/Debug.h"
#include "Core/Trackable.h"

template <class T>
class WeakPtr {
private:
    WeakPtr<Framework::Trackable> *prevRef;
    WeakPtr<Framework::Trackable> *nextRef;

    T *ptr;
public:
    WeakPtr();
    WeakPtr(T *p);
    WeakPtr(const WeakPtr<T> &p);
    WeakPtr(WeakPtr<T> &&p);
    ~WeakPtr();

    void Set(T *p);
    void Invalidate();
    bool IsValid() const;

    WeakPtr<T>& operator = (const WeakPtr<T> &p);
    WeakPtr<T>& operator = (T *p);

    bool operator == (const WeakPtr<T> &p) const;
    bool operator != (const WeakPtr<T> &p) const;
    bool operator == (const T *p) const;
    bool operator != (const T *p) const;

    operator T* () const;
    T* operator -> () const;
    T& operator * () const;

    template <class U> U* Cast() const;

    friend class Framework::Trackable;
};

template <class T>
WeakPtr<T>::WeakPtr()
: prevRef(nullptr),
  nextRef(nullptr),
  ptr(nullptr)
{ }

template <class T>
WeakPtr<T>::WeakPtr(T *p)
: prevRef(nullptr),
  nextRef(nullptr),
  ptr(p)
{
    if (nullptr != ptr) {
        WeakPtr<Framework::Trackable> *ref = reinterpret_cast<WeakPtr<Framework::Trackable>*>(this);

        static_cast<Framework::Trackable*>(ptr)->AddWeakRef(ref);
    }
}

template <class T>
WeakPtr<T>::WeakPtr(const WeakPtr<T> &p)
: prevRef(nullptr),
  nextRef(nullptr),
  ptr(p.ptr)
{
    if (nullptr != ptr) {
        WeakPtr<Framework::Trackable> *ref = reinterpret_cast<WeakPtr<Framework::Trackable>*>(this);

        static_cast<Framework::Trackable*>(ptr)->AddWeakRef(ref);
    }
}

template <class T>
WeakPtr<T>::WeakPtr(WeakPtr<T> &&p)
: prevRef(nullptr),
  nextRef(nullptr),
  ptr(p.ptr)
{
    p.Invalidate();

    if (nullptr != ptr) {
        WeakPtr<Framework::Trackable> *ref = reinterpret_cast<WeakPtr<Framework::Trackable>*>(this);

        static_cast<Framework::Trackable*>(ptr)->AddWeakRef(ref);
    }
}

template <class T>
WeakPtr<T>::~WeakPtr()
{
	Invalidate();
}

template <class T>
inline void
WeakPtr<T>::Set(T *p)
{
    WeakPtr<Framework::Trackable> *ref = reinterpret_cast<WeakPtr<Framework::Trackable>*>(this);

    if (nullptr != ptr)
        static_cast<Framework::Trackable*>(ptr)->RemoveWeakRef(ref);

    ptr = p;

	if (nullptr != ptr)
	{
		if (ptr->IsAlive())
			static_cast<Framework::Trackable*>(ptr)->AddWeakRef(ref);
		else
			ptr = nullptr;
	}
}

template <class T>
inline void
WeakPtr<T>::Invalidate()
{
    if (nullptr != ptr) {
        WeakPtr<Framework::Trackable> *ref = reinterpret_cast<WeakPtr<Framework::Trackable>*>(this);

        static_cast<Framework::Trackable*>(ptr)->RemoveWeakRef(ref);
    }

    ptr = nullptr;
}

template <class T>
inline bool
WeakPtr<T>::IsValid() const
{
	return (ptr != nullptr && ptr->IsAlive());
}

template <class T>
inline WeakPtr<T>&
WeakPtr<T>::operator = (const WeakPtr<T> &p)
{
    Set(p.ptr);
    return *this;
}

template <class T>
inline WeakPtr<T>&
WeakPtr<T>::operator = (T *p)
{
    Set(p);
    return *this;
}

template <class T>
inline bool
WeakPtr<T>::operator == (const WeakPtr<T> &p) const
{
    return (ptr == p.ptr);
}

template <class T>
inline bool
WeakPtr<T>::operator != (const WeakPtr<T> &p) const
{
    return (ptr != p.ptr);
}

template <class T>
inline bool
WeakPtr<T>::operator == (const T *p) const
{
    return (ptr == p);
}

template <class T>
inline bool
WeakPtr<T>::operator != (const T *p) const
{
    return (ptr != p);
}

template <class T>
inline
WeakPtr<T>::operator T* () const
{
    return ptr;
}

template <class T>
inline T*
WeakPtr<T>::operator -> () const
{
    assert(IsValid());
    return ptr;
}

template <class T>
inline T&
WeakPtr<T>::operator * () const
{
    assert(IsValid());
    return *ptr;
}

template <class T>
template <class U>
inline U*
WeakPtr<T>::Cast() const
{
    assert(IsValid());
    assert(ptr->template IsA<U>());
    return static_cast<U*>(ptr);
}
