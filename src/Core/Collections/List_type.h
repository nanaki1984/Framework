#pragma once

#include <cstdint>

namespace Framework {

template <typename T>
struct ListNode {
    T *prev;
    T *next;

    ListNode();
};

template <typename T, ListNode<T> T::* N>
class List
{
private:
    T *head;
    T *tail;
    uint32_t size;
public:
    List();
    List(const List<T, N> &other) = delete;
    List(List<T, N> &&other);
    ~List();

    List<T, N>& operator =(const List<T, N> &other) = delete;
    List<T, N>& operator =(List<T, N> &&other);

    uint32_t Count() const;
    bool IsEmpty() const;

    const T* Begin() const;
    T* Begin();
    const T* End() const;
    T* End();

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
    const T* GetPrevious(const T *node) const;
    T* GetPrevious(T *node);
};

} // namespace Framework
