#define CATCH_CONFIG_MAIN
#include "vendor/catch.hpp"

#include "Simple/StaticMap.hpp"

TEST_CASE("Insert to map using operator[]")
{
    Simple::StaticMap<std::string, std::string, 2> map;

    map["color"] = "#FFFFFF";
    REQUIRE(map["color"] == "#FFFFFF");
    REQUIRE(map.size() == 1);

    map["background-color"] = "#000000";
    REQUIRE(map["background-color"] == "#000000");
    REQUIRE(map.size() == 2);

    map.clear();
    REQUIRE(map.empty());
}

TEST_CASE("Insert to map using at() function")
{
    Simple::StaticMap<std::string, std::string, 2> map;

    REQUIRE_THROWS(map.at("color"));
    REQUIRE_NOTHROW(map["color"] = "#FFFFFF");
    REQUIRE_NOTHROW(map.at("color") == "#FFFFFF");
}
