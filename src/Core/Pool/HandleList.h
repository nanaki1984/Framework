#pragma once

#include "Core/Pool/HandleList_type.h"

namespace Framework {

template <typename T, HandleListNode<T> T::* N>
inline
HandleList<T, N>::HandleList()
: head(nullptr),
  tail(nullptr),
  size(0)
{ }

template <typename T, HandleListNode<T> T::* N>
inline
HandleList<T, N>::HandleList(HandleList<T, N> &&other)
: head(other.head),
  tail(other.tail),
  size(other.size)
{
    other.head = other.tail = nullptr;
    other.size = 0;
}

template <typename T, HandleListNode<T> T::* N>
inline
HandleList<T, N>::~HandleList()
{
    assert(0 == size);
}

template <typename T, HandleListNode<T> T::* N>
inline HandleList<T, N>&
HandleList<T, N>::operator =(HandleList<T, N> &&other)
{
    head = other.head;
    tail = other.tail;
    size = other.size;

    other.head = other.tail = nullptr;
    other.size = 0;

    return (*this);
}

template <typename T, HandleListNode<T> T::* N>
inline uint32_t
HandleList<T, N>::Count() const
{
    return size;
}

template <typename T, HandleListNode<T> T::* N>
inline bool
HandleList<T, N>::IsEmpty() const
{
    return 0 == size;
}

template <typename T, HandleListNode<T> T::* N>
inline const T*
HandleList<T, N>::Begin() const
{
    return head;
}

template <typename T, HandleListNode<T> T::* N>
inline T*
HandleList<T, N>::Begin()
{
    return head;
}

template <typename T, HandleListNode<T> T::* N>
inline const T*
HandleList<T, N>::End() const
{
    return tail;
}

template <typename T, HandleListNode<T> T::* N>
inline T*
HandleList<T, N>::End()
{
    return tail;
}

template <typename T, HandleListNode<T> T::* N>
inline const Handle<T>&
HandleList<T, N>::HandleBegin() const
{
    return head;
}

template <typename T, HandleListNode<T> T::* N>
inline const Handle<T>&
HandleList<T, N>::HandleEnd() const
{
    return tail;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::Clear()
{
    Handle<T> node = head, tmp;
    while (node != Handle<T>::Null) {
        tmp  = node;
        node = (node->*N).next;

        (tmp->*N).prev = Handle<T>::Null;
        (tmp->*N).next = Handle<T>::Null;
    }
    head = tail = Handle<T>::Null;
    size = 0;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::PushBack(T *node)
{
    if (Handle<T>::Null == tail) {
        assert(0 == size);
        head = tail = node;
        (node->*N).prev = Handle<T>::Null;
        (node->*N).next = Handle<T>::Null;
    } else {
        (tail->*N).next = node;
        (node->*N).prev = tail;
        (node->*N).next = Handle<T>::Null;
        tail = node;
    }
    ++size;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::PopBack()
{
    assert(tail != Handle<T>::Null);
    Handle<T> node = (tail->*N).prev;
    if (Handle<T>::Null == node) {
        assert(1 == size);
        head = tail = Handle<T>::Null;
    } else {
        (tail->*N).prev = Handle<T>::Null;
        tail = node;
    }
    --size;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::PushFront(T *node)
{
    if (Handle<T>::Null == head) {
        assert(0 == size);
        head = tail = node;
        (node->*N).prev = Handle<T>::Null;
        (node->*N).next = Handle<T>::Null;
    } else {
        (head->*N).prev = node;
        (node->*N).prev = Handle<T>::Null;
        (node->*N).next = head;
        head = node;
    }
    ++size;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::PopFront()
{
    assert(head != Handle<T>::Null);
    Handle<T> node = (head->*N).next;
    if (Handle<T>::Null == node) {
        assert(1 == size);
        head = tail = Handle<T>::Null;
    } else {
        (head->*N).next = Handle<T>::Null;
        head = node;
    }
    --size;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::InsertBefore(T *next, T *node)
{
    Handle<T> prev = (next->*N).prev;
    (next->*N).prev = node;
    (node->*N).prev = prev;
    (node->*N).next = next;
    if (prev != Handle<T>::Null)
        (prev->*N).next = node;
    else
        head = node;
    ++size;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::InsertAfter(T *prev, T *node)
{
    Handle<T> next = (prev->*N).next;
    (prev->*N).next = node;
    (node->*N).prev = prev;
    (node->*N).next = next;
    if (next != Handle<T>::Null)
        (next->*N).prev = node;
    else
        tail = node;
    ++size;
}

template <typename T, HandleListNode<T> T::* N>
inline void
HandleList<T, N>::Remove(T *node)
{
    Handle<T> prev = (node->*N).prev,
              next = (node->*N).next;

    (node->*N).prev = Handle<T>::Null;
    (node->*N).next = Handle<T>::Null;

    if (head.EqualsTo(node))
    {
        if (tail.EqualsTo(node))
        {
            assert(1 == size);
            head = tail = Handle<T>::Null;
        }
        else
        {
            head = next;
            assert(head.IsValid());
            (head->*N).prev = Handle<T>::Null;
        }
    }
    else if (tail.EqualsTo(node))
    {
        tail = prev;
        assert(tail.IsValid());
        (tail->*N).next = Handle<T>::Null;
    }
    else
    {
        assert(prev != Handle<T>::Null && next != Handle<T>::Null);
        (prev->*N).next = next;
        (next->*N).prev = prev;
    }

    assert(size > 0);
    --size;
}

template <typename T, HandleListNode<T> T::* N>
inline const T*
HandleList<T, N>::GetNext(const T *node) const
{
    return (node->*N).next;
}

template <typename T, HandleListNode<T> T::* N>
inline T*
HandleList<T, N>::GetNext(T *node)
{
    return (node->*N).next;
}

template <typename T, HandleListNode<T> T::* N>
inline const Handle<T>&
HandleList<T, N>::GetNext(const Handle<T> &node) const
{
    return ((node->*N).next);
}

template <typename T, HandleListNode<T> T::* N>
inline const T*
HandleList<T, N>::GetPrevious(const T *node) const
{
    return (node->*N).prev;
}

template <typename T, HandleListNode<T> T::* N>
inline T*
HandleList<T, N>::GetPrevious(T *node)
{
    return (node->*N).prev;
}

template <typename T, HandleListNode<T> T::* N>
inline const Handle<T>&
HandleList<T, N>::GetPrevious(const Handle<T> &node) const
{
    return ((node->*N).prev);
}

} // namespace Framework
