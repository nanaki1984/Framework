#pragma once

#include <type_traits>
#include <memory>
#include <cstring>

namespace Framework {
    namespace Memory {

template <typename T>
void Zero(T *dest)
{
    memset(dest, 0, sizeof(T));
}

template <typename T>
void Zero(T *dest, size_t n)
{
    memset(dest, 0, sizeof(T) * n);
}

template <typename T>
void Copy(T *dest, const T *src, size_t n)
{
    if (std::is_trivially_copyable<T>::value)
        memcpy(dest, src, sizeof(T) * n);
    else {
        for (size_t i = 0; i < n; ++i) {
            if (std::is_copy_constructible<T>::value)
                new(dest + i) T(*(src+ i));
            else {
                if (!std::is_trivially_default_constructible<T>::value)
                    new(dest + i) T();

                *(dest + i) = *(src + i);
            }
        }
    }
}

template <>
inline void Copy(void *dest, const void *src, size_t n)
{
    memcpy(dest, src, n);
}

template <typename T>
void Move(T *dest, T *src, size_t n)
{
    if (std::is_trivially_copyable<T>::value)
        memmove(dest, src, sizeof(T) * n);
    else {
        for (size_t i = 0; i < n; ++i) {
            if (std::is_move_constructible<T>::value)
                new(dest + i) T(std::forward<T>(*(src + i)));
            else if (std::is_move_assignable<T>::value) {
                if (!std::is_trivially_default_constructible<T>::value)
                    new(dest + i) T();

                *(dest + i) = std::forward<T>(*(src + i));
            } else if (std::is_copy_constructible<T>::value)
                new(dest + i) T(*(src + i));
            else {
                if (!std::is_trivially_default_constructible<T>::value)
                    new(dest + i) T();

                *(dest + i) = *(src + i);
            }

            if (std::is_destructible<T>::value)
                (src + i)->~T();
        }
    }
}

template <>
inline void Move(void *dest, void *src, size_t n)
{
    memmove(dest, src, n);
}

template <typename T>
void MoveReverse(T *dest, T *src, size_t n)
{
    if (std::is_trivially_copyable<T>::value) {
        for (int32_t i = n - 1; i >= 0; --i)
            memmove(dest + i, src + i, sizeof(T));
    } else {
        for (int32_t i = n - 1; i >= 0; --i) {
            if (std::is_move_constructible<T>::value)
                new(dest + i) T(std::forward<T>(*(src + i)));
            else if (std::is_move_assignable<T>::value) {
                if (!std::is_trivially_default_constructible<T>::value)
                    new(dest + i) T();
                
                *(dest + i) = std::forward<T>(*(src + i));
            } else if (std::is_copy_constructible<T>::value)
                new(dest + i) T(*(src + i));
            else {
                if (!std::is_trivially_default_constructible<T>::value)
                    new(dest + i) T();
                
                *(dest + i) = *(src + i);
            }
            
            if (std::is_destructible<T>::value)
                (src + i)->~T();
        }
    }
}

void InitializeMemory();
void ShutdownMemory();

template <typename A, typename ...Args> void InitAllocator(Args... params);
template <typename A> A& GetAllocator();

template <typename A, typename T, typename ...Args>
T* New(Args... params)
{
    return new (GetAllocator<A>().Allocate(sizeof(T), __alignof(T))) T(params...);
}

template <typename A, typename T>
void Delete(T *p)
{
    static_assert(std::is_destructible<T>::value, "Delete requires a dtor");
    p->~T();
    GetAllocator<A>().Free(p);
}

template <typename A, typename T>
T* NewArray(size_t size)
{
    static_assert(std::is_pod<T>::value, "NewArray requires POD types");
    return static_cast<T*>(GetAllocator<A>().Allocate(size * sizeof(T), __alignof(T)));
}

template <typename A, typename T>
void DeleteArray(T *p)
{
    static_assert(std::is_pod<T>::value, "DeleteArray requires POD types");
    GetAllocator<A>().Free(p);
}

    } // namespace Memory
} // namespace Framework
