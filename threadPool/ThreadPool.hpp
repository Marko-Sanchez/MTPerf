#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <stddef.h>
#include <functional>

class ThreadPool
{
private:
    std::atomic_bool done;
    std::mutex       qlock;
    size_t           _size;

    std::vector< std::thread >          threadpool;
    std::queue< std::function<void()> > workQueue;

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
    void submit(FunctionType func);
};
