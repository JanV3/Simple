#ifndef SIMPLE_SERIALIZE_HPP
#define SIMPLE_SERIALIZE_HPP

#include <istream>
#include <ostream>
#include <type_traits>

namespace Simple {
using std::size_t;

class SerializationArchive {
public:
    explicit SerializationArchive(std::ostream& stream)
        : stream_(stream)
    {
    }

    void write(const char* data, size_t size)
    {
        stream_.write(data, size);
    }

private:
    std::ostream& stream_;
};

class DeserializationArchive {
public:
    explicit DeserializationArchive(std::istream& stream)
        : stream_(stream)
    {
        stream_.seekg(0, stream_.beg);
    }

    void read(char* data, size_t size)
    {
        stream_.read(data, size);
    }

private:
    std::istream& stream_;
};

/**
 * @brief Serialize POD data
 */
template <class Archive, class T,
          class = typename std::enable_if<std::is_pod<T>::value>::type>
Archive& operator<<(Archive& archive, const T& obj)
{
    archive.write(reinterpret_cast<const char*>(&obj), sizeof(T));
    return archive;
}

/**
 * @brief Deserialize POD data
 */
template <class Archive, class T,
          class = typename std::enable_if<std::is_pod<T>::value>::type>
Archive& operator>>(Archive& archive, T& obj)
{
    archive.read(reinterpret_cast<char*>(&obj), sizeof(T));
    return archive;
}

/**
 * @brief Serialize basic_string
 */
template <class Archive, class CharacterType>
Archive& operator<<(Archive& archive, const std::basic_string<CharacterType>& string)
{
    auto size = string.size();
    archive << size;
    archive.write(reinterpret_cast<const char*>(string.data()),
                  size * sizeof(CharacterType));
    return archive;
}

/**
 * @brief Deserialize basic_string
 */
template <class Archive, class CharacterType>
Archive& operator>>(Archive& archive, std::basic_string<CharacterType>& string)
{
    size_t size{0};
    archive >> size;
    string.resize(size);
    archive.read(reinterpret_cast<char*>(string.data()), size * sizeof(CharacterType));
    return archive;
}

/**
 * @brief Serialize containers with specified item type and size of
 * container(std::array like)
 */
template <class Archive, template <class, size_t> class SizedContainer, class ItemType,
          size_t Size>
Archive& operator<<(Archive& archive, const SizedContainer<ItemType, Size>& items)
{
    auto size = items.size();
    archive << size;
    for (const auto& i : items) {
        archive << i;
    }
    return archive;
}

/**
 * @brief Deserialize containers with specified item type and size of
 * container(std::array like)
 */
template <class Archive, template <class, size_t> class SizedContainer, class ItemType,
          size_t Size>
Archive& operator>>(Archive& archive, SizedContainer<ItemType, Size>& items)
{
    size_t size{0};
    archive >> size;
    assert(size == items.size());
    for (size_t n = 0; n < size; ++n) {
        ItemType item;
        archive >> item;
        items[n] = std::move(item);
    }
    return archive;
}

/**
 * @brief Serialize containers with one primary template parameter(vector,list,set)
 */
template <class Archive, template <class> class Container, class ItemType>
Archive& operator<<(Archive& archive, const Container<ItemType>& items)
{
    auto size = items.size();
    archive << size;
    for (const auto& i : items) {
        archive << i;
    }
    return archive;
}

namespace detail {

template <class Container, class ItemType>
auto add_to_container(Container& items, ItemType&& item)
    -> decltype(items.push_back(std::move(item)))
{
    return items.push_back(std::move(item));
}

template <class Container, class ItemType>
auto add_to_container(Container& items, ItemType&& item)
    -> decltype(items.insert(std::move(item)))
{
    return items.insert(std::move(item));
}

} // namespace detail

/**
 * @brief Deserialize containers witch push_back or insert methods
 */
template <class Archive, template <class> class Container, class ItemType>
Archive& operator>>(Archive& archive, Container<ItemType>& items)
{
    size_t size{0};
    archive >> size;
    for (size_t n = 0; n < size; ++n) {
        ItemType item;
        archive >> item;
        detail::add_to_container(items, std::move(item));
    }
    return archive;
}

/**
 * @brief Serialize map containers
 */
template <class Archive, template <class, class> class Container, class KeyType,
          class ItemType>
typename std::enable_if<
    std::is_same<ItemType, typename Container<KeyType, ItemType>::mapped_type>::value,
    Archive&>::type
operator<<(Archive& archive, const Container<KeyType, ItemType>& items)
{
    auto size = items.size();
    archive << size;
    for (const auto& i : items) {
        archive << i.first;
        archive << i.second;
    }
    return archive;
}

/**
 * @brief Deserialize map containers
 */
template <class Archive, template <class, class> class Container, class KeyType,
          class ItemType>
typename std::enable_if<
    std::is_same<ItemType, typename Container<KeyType, ItemType>::mapped_type>::value,
    Archive&>::type
operator>>(Archive& archive, Container<KeyType, ItemType>& items)
{
    using ValueType = typename Container<KeyType, ItemType>::value_type;

    size_t size{0};
    archive >> size;
    for (size_t n = 0; n < size; ++n) {
        KeyType key;
        archive >> key;
        ItemType item;
        archive >> item;
        items.emplace(ValueType(std::move(key), std::move(item)));
    }
    return archive;
}

} // namespace Simple

#endif /* ifndef SIMPLE_SERIALIZE_HPP */

