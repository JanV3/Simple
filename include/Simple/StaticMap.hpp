#ifndef SIMPLE_STATIC_MAP_H
#define SIMPLE_STATIC_MAP_H

#include <cstdint>
#include <cstdlib>
#include <tuple>

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
    using pointer = T*;
    using const_pointer = const T*;
    using value_type = std::pair<const Key, T>;
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

        reference operator->()
        {
            return map_->items_[index_];
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

        iterator& operator+(int distance)
        {
            index_ += distance;
            return *this;
        }

        iterator& operator+=(int distance)
        {
            index_ += distance;
            return *this;
        }

        iterator& operator-(int distance)
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
            new (&storage_[size_++])
                value_type(std::make_pair((*first).first, (*first).second));
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

    std::pair<iterator, bool> emplace(value_type&& value)
    {
        auto findResult = findKey(value.first);
        if (findResult.second) {
            return std::make_pair(iterator{this, findResult.first}, false);
        }
        else {
            new (&storage_[size_]) value_type(
                std::make_pair(std::move(value.first), std::move(value.second)));
            return std::make_pair(iterator{this, size_++}, true);
        }
    }

    T& operator[](const Key& key)
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(storage_[o.first]);
        }
        new (&storage_[size_]) value_type(std::make_pair(key, T()));
        return std::get<1>(storage_[size_++]);
    }

    T& operator[](Key&& key)
    {
        auto o = findKey(key);
        if (o.second) {
            return std::get<1>(storage_[o.first]);
        }
        new (&storage_[size_]) value_type(std::make_pair(std::move(key), T()));
        return std::get<1>(storage_[size_++]);
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
