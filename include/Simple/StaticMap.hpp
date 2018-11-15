#ifndef SIMPLE_STATIC_MAP_H
#define SIMPLE_STATIC_MAP_H

#include <cstdint>
#include <cstdlib>
#include <tuple>

namespace Simple {
namespace detail {

template <class T, std::size_t Size>
class storage {
    static const size_t TypeSize = sizeof(T);
    static const size_t TypeAlign = alignof(T);
    static const size_t StorageSize = Size * TypeSize;
    static const size_t LastTypeBegin = StorageSize - TypeSize;

    using StorageType = typename std::aligned_storage<TypeSize, TypeAlign>::type;
    StorageType storage_[Size];

public:
    void* data()
    {
        return static_cast<void*>(&storage_);
    }

    T& operator[](size_t index)
    {
        return reinterpret_cast<T&>(storage_[index * TypeSize]);
    }

    const T& operator[](size_t index) const
    {
        return reinterpret_cast<const T&>(storage_[index * TypeSize]);
    }

    T& at(size_t index)
    {
        const size_t start = index * TypeSize;
        if (start > LastTypeBegin) {
            throw std::out_of_range("Cannot access type on given index.");
        }
        return reinterpret_cast<T&>(storage_[index * TypeSize]);
    }

    const T& at(size_t index) const
    {
        const size_t start = index * TypeSize;
        if (start > LastTypeBegin) {
            throw std::out_of_range("Cannot access type on given index.");
        }
        return reinterpret_cast<const T&>(storage_[index * TypeSize]);
    }
};
} // namespace detail

template <class Key, class T, std::size_t Capacity>
class StaticMap {
public:
    using key_type = Key;
    using mapped_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using value_type = std::pair<const Key, T>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;

    virtual ~StaticMap() noexcept
    {
        clear();
    }

    T& operator[](const Key& key)
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(items_[o.first]);
        }
        new (&items_[size_]) value_type(std::make_pair(key, T()));
        return std::get<1>(items_[size_++]);
    }

    T& at(const Key& key)
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(items_[o.first]);
        }
        throw std::out_of_range("Specified key not found");
    }

    const T& at(const Key& key) const
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(items_[o.first]);
        }
        throw std::out_of_range("Specified key not found");
    }

    void clear()
    {
        for (size_t i = 0; i < size_; ++i) {
            items_[i].~value_type();
        }
        size_ = 0;
    }

    size_type size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size_ == 0;
    }

private:
    std::pair<size_t, bool> findKey(const Key& key) const
    {
        for (size_t i = 0; i < size_; ++i) {
            if (key == std::get<0>(items_[i])) {
                return std::pair<size_t, bool>(i, true);
            }
        }
        return std::pair<size_t, bool>(0, false);
    }

    std::size_t size_{0};
    detail::storage<value_type, Capacity> items_;
};
} // namespace Simple

#endif /* ifndef SIMPLE_STATIC_MAP_H */
