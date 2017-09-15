#pragma once

#include <cstdint>
#include "Core/Pool/Handle_type.h"

namespace Framework {

template <typename T>
struct HandleListNode {
    Handle<T> prev;
    Handle<T> next;
};

template <typename T, HandleListNode<T> T::* N>
class HandleList
{
private:
	Handle<T> head;
    Handle<T> tail;
    uint32_t size;
public:
    HandleList();
    HandleList(const HandleList<T, N> &other) = delete;
    HandleList(HandleList<T, N> &&other);
    ~HandleList();

    HandleList<T, N>& operator =(const HandleList<T, N> &other) = delete;
    HandleList<T, N>& operator =(HandleList<T, N> &&other);

    uint32_t Count() const;
    bool IsEmpty() const;

    const T* Begin() const;
    T* Begin();
    const T* End() const;
    T* End();

    const Handle<T>& HandleBegin() const;
    const Handle<T>& HandleEnd() const;

    void Clear();

    void PushBack(T *node);
    void PopBack();

    void PushFront(T *node);
    void PopFront();

    void InsertBefore(T *next, T *node);
    void InsertAfter(T *prev, T *node);

    void Remove(T *node);

    const T* GetNext(const T *node) const;
    T* GetNext(T *node);
    const Handle<T>& GetNext(const Handle<T> &node) const;
    const T* GetPrevious(const T *node) const;
    T* GetPrevious(T *node);
    const Handle<T>& GetPrevious(const Handle<T> &node) const;
};

} // namespace Framework
