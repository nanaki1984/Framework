#pragma once

#include <mutex>
#include "Core/RefCounted.h"

template <class T> class WeakPtr;

namespace Framework {

class Trackable : public RefCounted {
    DeclareClassInfo;
protected:
    std::atomic_uint32_t weakRefCount;

    WeakPtr<Trackable> *head;
    WeakPtr<Trackable> *tail;

    std::recursive_mutex weakRefLock;

	virtual void InvalidateWeakReferences() final override;
public:
    Trackable();
    virtual ~Trackable();

    void AddWeakRef(WeakPtr<Trackable> *ptr);
    void RemoveWeakRef(WeakPtr<Trackable> *ptr);

	uint32_t GetWeakRefCount() const;
};

inline uint32_t
Trackable::GetWeakRefCount() const
{
    return weakRefCount;
}

} // namespace Framework
