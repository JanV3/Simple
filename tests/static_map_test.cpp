#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "Simple/StaticMap.hpp"

TEST_CASE("Insert to map using operator[]")
{
    Simple::StaticMap<std::string, std::string, 2> map;

    REQUIRE(map.capacity() == 2);
    REQUIRE(map.max_size() == 2);

    map["color"] = "#FFFFFF";
    REQUIRE(map["color"] == "#FFFFFF");
    REQUIRE(map.size() == 1);

    map["background-color"] = "#000000";
    REQUIRE(map["background-color"] == "#000000");
    REQUIRE(map.size() == 2);

    map.clear();
    REQUIRE(map.empty());
    REQUIRE(map.size() == 0);
}

TEST_CASE("Insert to map using at() function")
{
    Simple::StaticMap<std::string, std::string, 2> map;

    REQUIRE_THROWS(map.at("color"));
    REQUIRE_NOTHROW(map["color"] = "#FFFFFF");
    REQUIRE_NOTHROW(map.at("color") == "#FFFFFF");
}

TEST_CASE("Initialise by initialiser_list")
{
    Simple::StaticMap<std::string, std::string, 10> map{
        {"color1", "red"}, {"color2", "green"}, {"color3", "blue"}};
    REQUIRE(map.size() == 3);
    REQUIRE(map["color1"] == "red");
    REQUIRE(map["color2"] == "green");
    REQUIRE(map["color3"] == "blue");
}

TEST_CASE("Emplace")
{
    Simple::StaticMap<std::string, std::string, 10> map;

    auto inserted = map.emplace(std::make_pair("color1", "red"));
    REQUIRE(map.size() == 1);
    REQUIRE(inserted.second);
    REQUIRE(inserted.first->first == "color1");
    REQUIRE(inserted.first->second == "red");

    auto noinserted = map.emplace("color1", "blue");
    REQUIRE(map.size() == 1);
    REQUIRE_FALSE(noinserted.second);
    REQUIRE(inserted.first->first == "color1");
    REQUIRE(inserted.first->second == "red");
}

TEST_CASE("Iterate over items")
{
    Simple::StaticMap<std::string, std::string, 10> map{
        {"color1", "red"}, {"color2", "green"}, {"color3", "blue"}};

    size_t i{0};
    for (const auto& p : map) {
        if (i == 0) {
            REQUIRE(p.first == "color1");
            REQUIRE(p.second == "red");
        }
        if (i == 1) {
            REQUIRE(p.first == "color2");
            REQUIRE(p.second == "green");
        }
        if (i == 2) {
            REQUIRE(p.first == "color3");
            REQUIRE(p.second == "blue");
        }
        ++i;
    }
}

TEST_CASE("Erase map item")
{
    Simple::StaticMap<std::string, std::string, 5> map{
        {"color1", "red"},
        {"color2", "green"},
        {"color3", "blue"},
        {"color4", "very long color definition that dont fit into small string "
                   "optimisation static buffer"},
        {"color5", "yellow"}};

    map.erase("color2");

    REQUIRE((map.begin() + 1)->first == "color3");
}

TEST_CASE("Erase map range")
{
    Simple::StaticMap<std::string, std::string, 5> map{{"color1", "red"},
                                                       {"color2", "green"},
                                                       {"color3", "blue"},
                                                       {"color4", "orange"},
                                                       {"color5", "yellow"}};

    map.erase(map.begin() + 1, map.begin() + 3);

    REQUIRE((map.begin() + 1)->first == "color4");

    map.erase(map.begin(), map.end());
    REQUIRE(map.size() == 0);
}
