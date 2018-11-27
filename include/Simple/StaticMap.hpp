#ifndef SIMPLE_STATIC_MAP_H
#define SIMPLE_STATIC_MAP_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <tuple>
#include <type_traits>

namespace Simple {
namespace detail {

template <class T, std::size_t Capacity>
class storage {
    static const size_t TypeSize = sizeof(T);
    static const size_t TypeAlign = alignof(T);
    static const size_t StorageSize = Capacity * TypeSize;

    using StorageType = typename std::aligned_storage<TypeSize, TypeAlign>::type;
    StorageType storage_[Capacity];

public:
    static const size_t StorageTypeSize = sizeof(StorageType);

    void* data()
    {
        return static_cast<void*>(&storage_);
    }

    T& operator[](size_t index)
    {
        return reinterpret_cast<T&>(storage_[index]);
    }

    const T& operator[](size_t index) const
    {
        return reinterpret_cast<const T&>(storage_[index]);
    }

    T& at(size_t index)
    {
        if (index >= Capacity) {
            throw std::out_of_range("Cannot access type on given index.");
        }
        return reinterpret_cast<T&>(storage_[index]);
    }

    const T& at(size_t index) const
    {
        if (index >= Capacity) {
            throw std::out_of_range("Cannot access type on given index.");
        }
        return reinterpret_cast<const T&>(storage_[index]);
    }
};
} // namespace detail

template <class Key, class T, std::size_t Capacity>
class StaticMap {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<Key, T>;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;

private:
    using StorageType = detail::storage<value_type, Capacity>;

public:
    class iterator : public std::iterator<std::random_access_iterator_tag, value_type,
                                          size_t, pointer, reference> {
        StaticMap* map_{nullptr};
        size_t index_{0};

        explicit iterator(StaticMap* map, size_t index)
            : map_{map}
            , index_{index}
        {
        }

        friend class StaticMap;

    public:
        reference operator*()
        {
            return map_->storage_[index_];
        }

        pointer operator->()
        {
            return &(map_->storage_[index_]);
        }

        iterator& operator++()
        {
            ++index_;
            return *this;
        }

        iterator operator++(int)
        {
            auto it = *this;
            ++index_;
            return it;
        }

        iterator& operator--()
        {
            --index_;
            return *this;
        }

        iterator operator--(int)
        {
            auto it = *this;
            --index_;
            return it;
        }

        iterator operator+(int distance)
        {
            index_ += distance;
            return *this;
        }

        iterator& operator+=(int distance)
        {
            index_ += distance;
            return *this;
        }

        iterator operator-(int distance)
        {
            index_ -= distance;
            return *this;
        }

        iterator& operator-=(int distance)
        {
            index_ -= distance;
            return *this;
        }

        iterator& operator=(const iterator& rhs)
        {
            index_ = rhs.index_;
            map_ = rhs.map_;
            return *this;
        }

        friend size_t operator-(const iterator& lhs, const iterator& rhs)
        {
            return lhs.index_ - rhs.index_;
        }

        friend bool operator>(const iterator& lhs, const iterator& rhs)
        {
            return lhs.index_ > rhs.index_;
        }

        friend bool operator>=(const iterator& lhs, const iterator& rhs)
        {
            return lhs.index_ >= rhs.index_;
        }

        friend bool operator<(const iterator& lhs, const iterator& rhs)
        {
            return lhs.index_ < rhs.index_;
        }

        friend bool operator<=(const iterator& lhs, const iterator& rhs)
        {
            return lhs.index_ <= rhs.index_;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return lhs.index_ == rhs.index_;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return lhs.index_ != rhs.index_;
        }
    };

public:
    StaticMap() = default;

    template <class Iterator>
    StaticMap(Iterator first, Iterator last)
    {
        for (; first != last; ++first) {
            construct((*first).first, (*first).second);
        }
    }

    StaticMap(std::initializer_list<value_type> values)
        : StaticMap(values.begin(), values.end())
    {
    }

    virtual ~StaticMap() noexcept
    {
        clear();
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto value = create(std::forward<Args>(args)...);
        auto findResult = findKey(value.first);
        if (findResult.second) {
            return std::make_pair(iterator{this, findResult.first}, false);
        }
        else {
            auto index = construct(std::move(value));
            return std::make_pair(iterator{this, index}, true);
        }
    }

    T& operator[](const Key& key)
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(storage_[o.first]);
        }
        auto index = construct(key, T());
        return std::get<1>(storage_[index]);
    }

    T& operator[](Key&& key)
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(storage_[o.first]);
        }
        auto index = construct(std::move(key), T());
        return std::get<1>(storage_[index]);
    }

    T& at(const Key& key)
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(storage_[o.first]);
        }
        throw std::out_of_range("Specified key not found");
    }

    const T& at(const Key& key) const
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(storage_[o.first]);
        }
        throw std::out_of_range("Specified key not found");
    }

    void erase(const Key& key)
    {
        auto o = findKey(key);
        if (o.second) {
            erase_impl(o.first, 1);
        }
    }

    void erase(iterator position)
    {
        erase_impl(position.index_, 1);
    }

    void erase(iterator first, iterator last)
    {
        erase_impl(first.index_, std::distance(first, last));
    }

    void clear()
    {
        for (size_t i = 0; i < size_; ++i) {
            storage_[i].~value_type();
        }
        size_ = 0;
    }

    size_type size() const
    {
        return size_;
    }

    size_type max_size() const
    {
        return Capacity;
    }

    size_type capacity() const
    {
        return Capacity;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    iterator begin()
    {
        return iterator{this, 0};
    }

    iterator end()
    {
        return iterator{this, size_};
    }

    const iterator begin() const
    {
        return iterator{this, 0};
    }

    const iterator end() const
    {
        return iterator{this, size_};
    }

private:
    value_type create(const value_type& value) {
        return value;
    }

    template<typename ...Args>
    value_type create(Args&& ...args)
    {
        return value_type(std::forward<Args>(args)...);
    }

    void erase_impl(size_t index, size_t count)
    {
        auto ptr = &storage_[0];
        auto next = index + count;
        auto remaining = size_ - next;
        std::move(ptr + next, ptr + next + remaining, ptr + index);
        for (size_t i = next + remaining - count; i < size_; ++i) {
            storage_[i].~value_type();
        }
        size_ -= count;
    }

    template <class... Args>
    size_t construct(Args&&... args)
    {
        new (&storage_[size_]) value_type(std::forward<Args>(args)...);
        return size_++;
    }

    std::pair<size_t, bool> findKey(const Key& key) const
    {
        for (size_t i = 0; i < size_; ++i) {
            if (key == std::get<0>(storage_[i])) {
                return std::pair<size_t, bool>(i, true);
            }
        }
        return std::pair<size_t, bool>(0, false);
    }

    std::size_t size_{0};
    StorageType storage_;
};

} // namespace Simple

#endif /* ifndef SIMPLE_STATIC_MAP_H */
