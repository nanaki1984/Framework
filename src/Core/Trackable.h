#pragma once

#include <mutex>
#include "Core/RefCounted.h"

template <class T> class WeakPtr;

namespace Framework {

class Trackable : public RefCounted {
    DeclareClassInfo;
protected:
    uint32_t weakRefCount;

    WeakPtr<Trackable> *head;
    WeakPtr<Trackable> *tail;

    std::recursive_mutex weakRefLock;
public:
    Trackable();
    virtual ~Trackable();

    void AddWeakRef(WeakPtr<Trackable> *ptr);
    void RemoveWeakRef(WeakPtr<Trackable> *ptr);
    void InvalidateAllWeakRefs();

	uint32_t GetWeakRefCount() const;
};

inline uint32_t
Trackable::GetWeakRefCount() const
{
    return weakRefCount;
}

} // namespace Framework
