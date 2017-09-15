#pragma once

#include "Core/Pool/BasePool.h"

template <typename T> class Handle;

namespace Framework {

template <typename T>
class Pool : public BasePool {
protected:
    virtual void CopyObjects(void *dest, void *src, uint32_t objectsCount);
    virtual void MoveObjects(void *dest, void *src, uint32_t objectsCount);
public:
    Pool(Allocator &allocator);
    Pool(const Pool<T> &other);
    Pool(Pool<T> &&other);
    virtual ~Pool();

    T* Begin();
    const T* Begin() const;
    T* End();
    const T* End() const;

    Handle<T> NewInstance();
    template <typename... Args> Handle<T> NewInstance(Args... arguments);
    Handle<T> GetInstance(uint32_t id);
    Handle<T> CloneInstance(const Handle<T> &handle);
    void DeleteInstance(const Handle<T> &handle);
};

} // namespace Framework
