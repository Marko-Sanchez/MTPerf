#include <cctype>
#include <cstdlib>
#include <iostream>

#include <chrono>
#include <ostream>
#include <ratio>

#include "threadpool.hpp"

int command_line_parser(int argc, char *argv[])
{
    if ( argc == 2)
    {
        auto thread_count = std::atoi(argv[1]);
        if (thread_count == 0)
        {
            std::cerr << "Invalid thread count provided" << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (thread_count > 64)
        {
            std::cerr << "Thread count is too high" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "Using thread count of " << thread_count << std::endl;
        return thread_count;
    }
    else if ( argc > 3)
    {
        std::cerr << "To many arguements" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Using default thread count of 2" << std::endl;
        return 2;
    }
}

int main(int argc, char *argv[])
{
    auto thread_count = command_line_parser(argc, argv);
    THREAD_POOL pool(thread_count);

    // warm up run
    for (int i{1}; i <= 1024; ++i)
    {
        pool.enqueue([](int x) {return x*x;}, i);
    }

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    // Code to be timed
    for (int i{1}; i <= 1024; ++i)
    {
        pool.enqueue([](int x) {return x*(x + x);}, i);
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::cout << "Time taken: " << time_span.count() << " (ms)" << std::endl;

    return EXIT_SUCCESS;
}
