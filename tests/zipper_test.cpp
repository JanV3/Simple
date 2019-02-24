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

    for (auto [i, c] : Simple::Zipper(ints, chars)) {
        std::cout << "int: " << i << " char: " << c << std::endl;
    }

    return 0;
}
