#include <iostream>
#include <thread>

#include "Simple/Logger.hpp"
#include "Simple/ThreadPool.hpp"

using namespace std;

int main()
{
    Simple::ThreadPool pool(5);

    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < 2; ++i) {
        futures.push_back(pool.addTask(
            [](size_t id) {
                size_t s = 0;
                for (; s < 100; ++s) {
                    ;
                }
                LOG_INFO << "Task ID " << id << " result = " << s;

            },
            i));
    }

    for (auto& f : futures) {
        f.get();
    }

    Simple::ThreadPool pool2(5);

    std::vector<std::future<size_t>> futures2;
    for (size_t i = 0; i < 2; ++i) {
        futures2.push_back(pool2.addTask(
            [](size_t id) {

                size_t s = 0;
                for (; s < 100; ++s) {
                    ;
                }
                LOG_INFO << "Task ID " << id << " result = " << s;
                return s;
            },
            i));
    }

    for (auto& f : futures2) {
        auto s = f.get();
        LOG_INFO << "REsult: " << s;
    }

    return 0;
}
