#include <iostream>
#include <thread>
#include <chrono>
#include "threadPool.hpp"

using std::cout, std::endl;

void foo()
{
    printf("Hello World\n");
}

int main (int argc, char *argv[])
{
    ThreadPool threadpool;

    cout << "Total Amount of default threads: " << threadpool.size() << endl;

    auto func = [](){printf("Hello World\n");};

    threadpool.submit(foo);
    threadpool.submit(func);

    auto addI = []()
    {
        const int maxValue{100};
        int val{0};
        for(int i = 0; i < maxValue; ++i)
        {
            val += i;
        }

        printf("value is %d\n", val);
    };

    threadpool.submit(addI);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    return 0;
}
