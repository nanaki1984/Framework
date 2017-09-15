#pragma once

#include "Core/Debug.h"
#include "Core/RefCounted.h"

namespace Framework {

template <class T>
class Singleton : public RefCounted {
private:
    static T *ptr;
public:
    Singleton();
    virtual ~Singleton();

    static T* Instance();
    static T* InstanceUnsafe();
};

template <class T> T *Singleton<T>::ptr = nullptr;

template <class T>
Singleton<T>::Singleton()
{
    assert(nullptr == ptr);
    ptr = static_cast<T*>(this);
}

template <class T>
Singleton<T>::~Singleton()
{
    assert(nullptr != ptr);
    ptr = nullptr;
}

template <class T>
inline T*
Singleton<T>::Instance()
{
    assert(nullptr != ptr);
    return ptr;
}

template <class T>
inline T*
Singleton<T>::InstanceUnsafe()
{
    return ptr;
}

}; // namespace Framework
