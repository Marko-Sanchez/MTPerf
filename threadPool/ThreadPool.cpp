#include "ThreadPool.hpp"

ThreadPool::ThreadPool()
:done(false), _size(std::thread::hardware_concurrency())
{
    InitializeThreads();
}

ThreadPool::ThreadPool(size_t size)
:done(false), _size(size)
{
    InitializeThreads();
}

ThreadPool::~ThreadPool()
{
    done = true;
    for(size_t i = 0; i < _size; ++i)
    {
        if(threadpool[i].joinable())
            threadpool[i].join();
    }
}

void ThreadPool::InitializeThreads()
{
    try
    {
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
}

void ThreadPool::work()
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
}

size_t ThreadPool::size()
{
    return _size;
}

template<typename FunctionType>
void ThreadPool::submit(FunctionType func)
{
    std::lock_guard<std::mutex> guard(qlock);
    workQueue.push(std::function<void()>(func));
}
