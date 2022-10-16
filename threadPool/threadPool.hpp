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
    const size_t     _size;

    std::vector< std::thread >          threadpool;
    std::queue< std::function<void()> > workQueue;

    void InitializeThreads()
    {
        try{
            for(size_t i = 0; i < _size; ++i)
            {
                threadpool.push_back(std::thread(&ThreadPool::work, this));
            }
        }
        catch(...)
        {
            done = true;
            throw;
        }
    };

    void work()
    {
        while(!done)
        {
            std::function<void()> job;
            {
                std::lock_guard<std::mutex> guard(qlock);
                if(!workQueue.empty())
                {
                    job = workQueue.front();
                    workQueue.pop();
                }
            }

            if(job)
            {
                job();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    };

public:

    ThreadPool()
    :done(false), _size(std::thread::hardware_concurrency())
    {
        InitializeThreads();
    };

    ThreadPool(size_t size)
    :done(false), _size(size)
    {
        InitializeThreads();
    };

    ~ThreadPool()
    {
        done = true;
        for(size_t i = 0; i < _size; ++i)
        {
            if(threadpool[i].joinable())
                threadpool[i].join();
        }
    };

    template<typename FunctionType>
    void submit(FunctionType f)
    {
        std::lock_guard<std::mutex> guard(qlock);
        workQueue.push(std::function<void()>(f));
    };

    size_t size()
    {
        return _size;
    };
};
