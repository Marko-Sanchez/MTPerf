#include <iostream>
#include <thread>
#include <chrono>
#include "ThreadPool.cpp"

using std::cout, std::endl;

auto addI = []()
{
    const int maxValue{10000};
    int val{0};

    for(int i = 0; i < maxValue; ++i)
    {
        val += i;
    }

    printf("int value is %d\n", val);
};

void addL()
{
    const long maxValue{10000};
    long val{0};

    for(long i = 0; i < maxValue; ++i)
    {
        val += i;
    }

    printf("long value is %ld\n", val);
}

void addD()
{
    const double maxValue{10000};
    double val{0};

    for(double i = 0; i < maxValue; ++i)
    {
        val += i;
    }

    printf("double value is %.2lf\n", val);
}

int main (int argc, char *argv[])
{
    ThreadPool threadpool(1);

    cout << "Total Amount of default threads: " << threadpool.size() << endl;

    threadpool.submit(addI);
    threadpool.submit(addL);
    threadpool.submit(addD);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    return 0;
}
