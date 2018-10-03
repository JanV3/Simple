#include "Simple/Zipper.hpp"

#include <iostream>
#include <list>
#include <thread>
#include <vector>

using namespace std;

int main()
{
    std::vector<int> ints{10, 20, 30, 40};
    std::list<char> chars{'a', 'b', 'c', 'd'};

    auto zipped = Simple::Zipper(ints, chars);

    for (auto [i, c] : zipped) {
        std::cout << "int: " << i << " char: " << c << std::endl;
    }

    return 0;
}
