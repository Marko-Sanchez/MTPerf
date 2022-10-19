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
        FunctionWrapper job;
        bool            hasJob{false};
        {
            std::lock_guard guard(qlock);
            if(!workQueue.empty())
            {
                // TODO: This will be updated with a better method of obtaining move-only object
                job = std::move(workQueue.front());
                workQueue.pop();
                hasJob = true;
            }
        }

        if(hasJob)
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
std::future<typename std::result_of_t<FunctionType()>>
ThreadPool::submit(FunctionType func)
{
    using result_type = std::result_of_t<FunctionType()>;

    std::packaged_task<result_type()> task(std::move(func));
    std::future<result_type> retFut(task.get_future());

    std::lock_guard guard(qlock);
    workQueue.push(std::move(task));

    return retFut;
}
