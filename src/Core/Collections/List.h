#pragma once

#include "Core/Collections/List_type.h"

namespace Framework {

template <typename T>
inline
ListNode<T>::ListNode()
: prev(nullptr),
  next(nullptr)
{ }

template <typename T, ListNode<T> T::* N>
inline
List<T, N>::List()
: head(nullptr),
  tail(nullptr),
  size(0)
{ }

template <typename T, ListNode<T> T::* N>
inline
List<T, N>::List(List<T, N> &&other)
: head(other.head),
  tail(other.tail),
  size(other.size)
{
    other.head = other.tail = nullptr;
    other.size = 0;
}

template <typename T, ListNode<T> T::* N>
inline
List<T, N>::~List()
{
    assert(0 == size);
}

template <typename T, ListNode<T> T::* N>
inline List<T, N>&
List<T, N>::operator =(List<T, N> &&other)
{
    head = other.head;
    tail = other.tail;
    size = other.size;

    other.head = other.tail = nullptr;
    other.size = 0;

    return (*this);
}

template <typename T, ListNode<T> T::* N>
inline uint32_t
List<T, N>::Count() const
{
    return size;
}

template <typename T, ListNode<T> T::* N>
inline bool
List<T, N>::IsEmpty() const
{
    return 0 == size;
}

template <typename T, ListNode<T> T::* N>
inline const T*
List<T, N>::Begin() const
{
    return head;
}

template <typename T, ListNode<T> T::* N>
inline T*
List<T, N>::Begin()
{
    return head;
}

template <typename T, ListNode<T> T::* N>
inline const T*
List<T, N>::End() const
{
    return tail;
}

template <typename T, ListNode<T> T::* N>
inline T*
List<T, N>::End()
{
    return tail;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::Clear()
{
    T *node = head, *tmp;
    while (node != nullptr) {
        tmp  = node;
        node = (node->*N).next;

        (tmp->*N).prev = nullptr;
        (tmp->*N).next = nullptr;
    }
    head = tail = nullptr;
    size = 0;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::PushBack(T *node)
{
    if (nullptr == tail) {
        assert(0 == size);
        head = tail = node;
        (node->*N).prev = nullptr;
        (node->*N).next = nullptr;
    } else {
        (tail->*N).next = node;
        (node->*N).prev = tail;
        (node->*N).next = nullptr;
        tail = node;
    }
    ++size;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::PopBack()
{
    assert(tail != nullptr);
    T *node = (tail->*N).prev;
    if (nullptr == node) {
        assert(1 == size);
        head = tail = nullptr;
    } else {
        (tail->*N).prev = nullptr;
        tail = node;
    }
    --size;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::PushFront(T *node)
{
    if (nullptr == head) {
        assert(0 == size);
        head = tail = node;
        (node->*N).prev = nullptr;
        (node->*N).next = nullptr;
    } else {
        (head->*N).prev = node;
        (node->*N).prev = nullptr;
        (node->*N).next = head;
        head = node;
    }
    ++size;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::PopFront()
{
    assert(head != nullptr);
    T *node = (head->*N).next;
    if (nullptr == node) {
        assert(1 == size);
        head = tail = nullptr;
    } else {
        (head->*N).next = nullptr;
        head = node;
    }
    --size;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::InsertBefore(T *next, T *node)
{
    T *prev = (next->*N).prev;
    (next->*N).prev = node;
    (node->*N).prev = prev;
    (node->*N).next = next;
    if (prev != nullptr)
        (prev->*N).next = node;
    else
        head = node;
    ++size;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::InsertAfter(T *prev, T *node)
{
    T *next = (prev->*N).next;
    (prev->*N).next = node;
    (node->*N).prev = prev;
    (node->*N).next = next;
    if (next != nullptr)
        (next->*N).prev = node;
    else
        tail = node;
    ++size;
}

template <typename T, ListNode<T> T::* N>
inline void
List<T, N>::Remove(T *node)
{
    T *prev = (node->*N).prev,
      *next = (node->*N).next;

    (node->*N).prev = nullptr;
    (node->*N).next = nullptr;

    if (head == node)
    {
        if (tail == node)
        {
            assert(1 == size);
            head = tail = nullptr;
        }
        else
        {
            head = next;
            assert(head != nullptr);
            (head->*N).prev = nullptr;
        }
    }
    else if (tail == node)
    {
        tail = prev;
        assert(tail != nullptr);
        (tail->*N).next = nullptr;
    }
    else
    {
        assert(prev != nullptr && next != nullptr);
        (prev->*N).next = next;
        (next->*N).prev = prev;
    }

    assert(size > 0);
    --size;
}

template <typename T, ListNode<T> T::* N>
inline const T*
List<T, N>::GetNext(const T *node) const
{
    return (node->*N).next;
}

template <typename T, ListNode<T> T::* N>
inline T*
List<T, N>::GetNext(T *node)
{
    return (node->*N).next;
}

template <typename T, ListNode<T> T::* N>
inline const T*
List<T, N>::GetPrevious(const T *node) const
{
    return (node->*N).prev;
}

template <typename T, ListNode<T> T::* N>
inline T*
List<T, N>::GetPrevious(T *node)
{
    return (node->*N).prev;
}

} // namespace Framework
