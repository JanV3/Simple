#include <iostream>
#include <thread>

#include "Simple/Signal.hpp"

using namespace std;

class Object {
public:
    int id = 0;

    Object(void)
    {
        static int counter = 0;
        id = ++counter;
        std::cout << "Object " << id << " constructed." << std::endl;
    }

    ~Object(void)
    {
        std::cout << "Object " << id << " destructed." << std::endl;
    }

public:
    void fun()
    {
        std::cout << "fun() member id = " << id << std::endl;
    }
};

int main()
{
    Simple::Signal<void()> signal;
    {
        for (auto i = 0; i < 5; ++i) {
            Object ob1;
            signal.connect(std::bind(&Object::fun, &ob1));
            signal();
            signal.disconnectAll();
        }
    }
    signal.disconnectAll();

    signal.connect([]() { std::cout << "f1() called" << std::endl; });
    signal();

    return 0;
}
