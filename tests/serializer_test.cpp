#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <array>
#include <list>
#include <set>
#include <unordered_map>
#include <vector>

#include "../include/Simple/Serialize.hpp"
using namespace Simple;

template <class T>
void deserialize_test(DeserializationArchive& d, T value)
{
    T i;
    d >> i;
    REQUIRE(i == value);
}

template <class T>
void deserialize_test(DeserializationArchive& d, std::vector<T> value)
{
    std::vector<T> i;
    d >> i;
    REQUIRE(i.size() == value.size());
    for (size_t n = 0; n < value.size(); ++n) {
        REQUIRE(i[n] == value[n]);
    }
}

template <class T, size_t S>
void deserialize_test(DeserializationArchive& d, std::array<T, S> value)
{
    std::array<T, S> i;
    d >> i;
    REQUIRE(i.size() == value.size());
    for (size_t n = 0; n < value.size(); ++n) {
        REQUIRE(i[n] == value[n]);
    }
}

template <class T>
void deserialize_test(DeserializationArchive& d, std::list<T> value)
{
    std::list<T> i;
    d >> i;

    REQUIRE(i.size() == value.size());
    for (const auto& v : value) {
        auto iter = std::find(i.begin(), i.end(), v);
        REQUIRE(iter != i.end());
    }
}

template <class T>
void deserialize_test(DeserializationArchive& d, std::set<T> value)
{
    std::set<T> i;
    d >> i;
    REQUIRE(i.size() == value.size());
    for (const auto& v : value) {
        auto iter = i.find(v);
        REQUIRE(iter != i.end());
    }
}

template <class K, class T>
void deserialize_test(DeserializationArchive& d, std::map<K, T> value)
{
    std::map<K, T> i;
    d >> i;
    REQUIRE(i.size() == value.size());
    for (const auto& v : value) {
        auto iter = i.find(v.first);
        REQUIRE(iter != i.end());
    }
}

template <class K, class T>
void deserialize_test(DeserializationArchive& d, std::unordered_map<K, T> value)
{
    std::map<K, T> i;
    d >> i;
    REQUIRE(i.size() == value.size());
    for (const auto& v : value) {
        auto iter = i.find(v.first);
        REQUIRE(iter != i.end());
    }
}

TEST_CASE("Serialize/deserialize POD data")
{
    std::stringstream ss;

    // serialize
    SerializationArchive s(ss);
    s << 5;
    s << 5.5;
    s << 5U;
    s << 5l;
    s << 5.5f;

    // deserialize
    DeserializationArchive d(ss);
    deserialize_test(d, 5);
    deserialize_test(d, 5.5);
    deserialize_test(d, 5U);
    deserialize_test(d, 5l);
    deserialize_test(d, 5.5f);
}

TEST_CASE("Serialize/deserialize std::string and std::wstring")
{
    std::stringstream ss;

    // serialize
    SerializationArchive s(ss);
    s << std::string("ahoj");
    s << std::wstring(L"ahoj");

    // deserialize
    DeserializationArchive d(ss);
    deserialize_test(d, std::string("ahoj"));
    deserialize_test(d, std::wstring(L"ahoj"));
}

TEST_CASE("Serialize/deserialize std::vector of plain data")
{
    std::stringstream ss;

    SerializationArchive s(ss);
    s << std::vector<int>(4, 5);
    s << std::vector<float>(4, 5.5f);
    s << std::vector<double>(4, 5.5);

    DeserializationArchive d(ss);
    deserialize_test(d, std::vector<int>(4, 5));
    deserialize_test(d, std::vector<float>(4, 5.5f));
    deserialize_test(d, std::vector<double>(4, 5.5));
}

TEST_CASE("Serialize/deserialize std::vector of std::string/std::wstring")
{
    std::stringstream ss;
    SerializationArchive s(ss);

    s << std::vector<std::string>(4, "ahoj");
    s << std::vector<std::wstring>(4, L"ahoj");

    DeserializationArchive d(ss);
    deserialize_test(d, std::vector<std::string>(4, "ahoj"));
    deserialize_test(d, std::vector<std::wstring>(4, L"ahoj"));
}

TEST_CASE("Serialize/deserialize std::array of plain data")
{
    std::stringstream ss;

    SerializationArchive s(ss);
    s << std::array<int, 2>{4, 5};
    s << std::array<float, 2>{4.0f, 5.0f};
    s << std::array<double, 2>{4.0, 5.0};

    DeserializationArchive d(ss);
    deserialize_test(d, std::array<int, 2>{4, 5});
    deserialize_test(d, std::array<float, 2>{4.0f, 5.0f});
    deserialize_test(d, std::array<double, 2>{4, 5.0});
}

TEST_CASE("Serialize/deserialize std::array of std::string/std::wstring")
{
    std::stringstream ss;
    SerializationArchive s(ss);

    s << std::array<std::string, 2>{"ahoj", "ahoj"};
    s << std::array<std::wstring, 2>{L"ahoj", L"ahoj"};

    DeserializationArchive d(ss);
    deserialize_test(d, std::array<std::string, 2>{"ahoj", "ahoj"});
    deserialize_test(d, std::array<std::wstring, 2>{L"ahoj", L"ahoj"});
}

TEST_CASE("Serialize/deserialize std::list of plain data")
{
    std::stringstream ss;

    SerializationArchive s(ss);
    s << std::list<int>(4, 5);
    s << std::list<float>(4, 5.5f);
    s << std::list<double>(4, 5.5);

    DeserializationArchive d(ss);
    deserialize_test(d, std::list<int>(4, 5));
    deserialize_test(d, std::list<float>(4, 5.5f));
    deserialize_test(d, std::list<double>(4, 5.5));
}

TEST_CASE("Serialize/deserialize std::list of std::string/std::wstring")
{
    std::stringstream ss;
    SerializationArchive s(ss);

    s << std::list<std::string>(4, "ahoj");
    s << std::list<std::wstring>(4, L"ahoj");

    DeserializationArchive d(ss);
    deserialize_test(d, std::list<std::string>(4, "ahoj"));
    deserialize_test(d, std::list<std::wstring>(4, L"ahoj"));
}

TEST_CASE("Serialize/deserialize std::set of plain data")
{
    std::stringstream ss;

    SerializationArchive s(ss);
    s << std::set<int>{1, 2, 3};
    s << std::set<float>{1.0f, 2.0f, 3.0f};
    s << std::set<double>{1.0, 2.0, 3.0};

    DeserializationArchive d(ss);
    deserialize_test(d, std::set<int>{1, 2, 3});
    deserialize_test(d, std::set<float>{1.0f, 2.0f, 3.0f});
    deserialize_test(d, std::set<double>{1.0, 2.0, 3.0});
}

TEST_CASE("Serialize/deserialize std::set of std::string/std::wstring")
{
    std::stringstream ss;
    SerializationArchive s(ss);

    s << std::set<std::string>{"a", "h", "o", "j"};
    s << std::set<std::wstring>{L"a", L"h", L"o", L"j"};

    DeserializationArchive d(ss);
    deserialize_test(d, std::set<std::string>{"a", "h", "o", "j"});
    deserialize_test(d, std::set<std::wstring>{L"a", L"h", L"o", L"j"});
}

TEST_CASE("Serialize/deserialize std::map")
{
    std::stringstream ss;
    SerializationArchive s(ss);

    s << std::map<std::string, std::string>({{"a", "b"}, {"c", "d"}});

    DeserializationArchive d(ss);
    deserialize_test(d, std::map<std::string, std::string>({{"a", "b"}, {"c", "d"}}));
}

TEST_CASE("Serialize/deserialize std::unordered_map")
{
    std::stringstream ss;
    SerializationArchive s(ss);

    s << std::unordered_map<std::string, std::string>({{"a", "b"}, {"c", "d"}});

    DeserializationArchive d(ss);
    deserialize_test(
        d, std::unordered_map<std::string, std::string>({{"a", "b"}, {"c", "d"}}));
}

struct CustomType {
    using Ints = std::vector<int>;
    Ints ints;
    using Map = std::map<size_t, Ints>;
    Map map;
};

template <class Archive>
Archive& operator<<(Archive& ar, const CustomType& type)
{
    ar << type.ints << type.map;
    return ar;
}

template <class Archive>
Archive& operator>>(Archive& ar, CustomType& type)
{
    ar >> type.ints >> type.map;
    return ar;
}

TEST_CASE("Serialize/deserialize custom type")
{
    CustomType t{{1, 2, 3}, {{1, {1, 2, 3}}, {2, {4, 5, 6}}}};
    std::stringstream ss;
    SerializationArchive s(ss);
    s << t;

    DeserializationArchive d(ss);
    CustomType deserialized;
    d >> deserialized;

    REQUIRE(t.ints.size() == deserialized.ints.size());
    REQUIRE(t.ints[0] == deserialized.ints[0]);
    REQUIRE(t.ints[1] == deserialized.ints[1]);
    REQUIRE(t.ints[2] == deserialized.ints[2]);

    REQUIRE(t.map.size() == deserialized.map.size());

    REQUIRE(t.map[1].size() == deserialized.map[1].size());
    REQUIRE(t.map[1][0] == deserialized.map[1][0]);
    REQUIRE(t.map[1][1] == deserialized.map[1][1]);
    REQUIRE(t.map[1][2] == deserialized.map[1][2]);

    REQUIRE(t.map[2].size() == deserialized.map[2].size());
    REQUIRE(t.map[2][0] == deserialized.map[2][0]);
    REQUIRE(t.map[2][1] == deserialized.map[2][1]);
    REQUIRE(t.map[2][2] == deserialized.map[2][2]);
}
