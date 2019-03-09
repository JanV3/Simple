#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "Simple/Zipper.hpp"

TEST_CASE("Create map from two containers")
{
    std::vector<int> ints{10, 20, 30, 40};
    std::list<char> chars{'a', 'b', 'c', 'd'};

    std::unordered_map<int, char> mapped;
    for (auto [i, c] : Simple::Zipper(ints, chars)) {
        mapped[i] = c;
    }

    REQUIRE(ints.size() == 4);
    REQUIRE(chars.size() == 4);
    REQUIRE(mapped.size() == 4);
    REQUIRE(mapped[10] == 'a');
    REQUIRE(mapped[20] == 'b');
    REQUIRE(mapped[30] == 'c');
    REQUIRE(mapped[40] == 'd');
}
