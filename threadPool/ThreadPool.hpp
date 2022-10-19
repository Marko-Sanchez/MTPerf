#pragma once
#include <thread>
#include <type_traits>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <stddef.h>
#include <future>

#include "FunctionWrapper.hpp"

class ThreadPool
{
private:
    std::atomic_bool done;
    std::mutex       qlock;
    size_t           _size;

    std::vector< std::thread >          threadpool;
    std::queue< FunctionWrapper >       workQueue;

    /* Creates and launches threads to do work. */
    void InitializeThreads();

    /* Threads wait for jobs and execute them. */
    void work();

public:

    ThreadPool();

    ThreadPool(size_t size);

    ~ThreadPool();

    /* returns amount of active threads */
    size_t size();

    /* submit task to work queue. */
    template<typename FunctionType>
    std::future<typename std::result_of_t<FunctionType()>> submit(FunctionType func);
};
