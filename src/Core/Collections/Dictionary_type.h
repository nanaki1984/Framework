#pragma once

#include "Core/Collections/Array_type.h"

namespace Framework {

template <typename K, typename V>
struct KeyValuePair {
    K key;
    V value;

    KeyValuePair();
    KeyValuePair(const K &k);
    KeyValuePair(const K &k, const V &v);
    KeyValuePair(const KeyValuePair<K, V> &other);
    KeyValuePair(KeyValuePair<K, V> &&other);
    ~KeyValuePair();

    KeyValuePair<K, V>& operator =(const KeyValuePair<K, V> &other);
    KeyValuePair<K, V>& operator =(KeyValuePair<K, V> &&other);

    bool operator <(const KeyValuePair<K, V> &other) const;
    bool operator >(const KeyValuePair<K, V> &other) const;
};

template <typename K, typename V>
class Dictionary {
private:
    static std::function<int(const KeyValuePair<K, V> &, const KeyValuePair<K, V> &)> sortFunction;

    Array<KeyValuePair<K, V>> data;
    bool bulkAdding;
public:
    Dictionary(Allocator &allocator);
    Dictionary(Allocator &allocator, uint32_t initialCapacity);
    Dictionary(const Dictionary<K, V> &other);
    Dictionary(Dictionary<K, V> &&other);
    ~Dictionary();

    Dictionary<K, V>& operator =(const Dictionary<K, V> &other);
    Dictionary<K, V>& operator =(Dictionary<K, V> &&other);

    const V& operator [](const K &key) const;
    V& operator [](const K &key);

    Allocator& GetAllocator() const;
    uint32_t Count() const;
    bool IsEmpty() const;

    const KeyValuePair<K, V>* Begin() const;
    KeyValuePair<K, V>* Begin();
    const KeyValuePair<K, V>* End() const;
    KeyValuePair<K, V>* End();

    bool Contains(const K &key) const;
    bool TryGetValue(const K &key, V &value) const;

    void Clear();
    void Add(const K &key, const V &value);
    void Remove(const K &key);

    void BeginBulkAdd();
    void BulkAdd(const K &key, const V &value);
    void EndBulkAdd();
};

template <typename K, typename V>
static int32_t BinarySearch(const Array<KeyValuePair<K, V>> &array, uint32_t index, uint32_t count, const K &item);

} // namespace Framework
