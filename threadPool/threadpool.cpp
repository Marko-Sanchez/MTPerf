#include "threadpool.hpp"

THREAD_POOL::THREAD_POOL(int thread_count): _stop(false)
{
    for (int i {0}; i < thread_count; ++i)
    {
        _threads.emplace_back([this]{
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->_qmutex);
                    this->_condition.wait(lock, [this]{return this->_stop.load() || !_tasks.empty();});
                    if (this->_stop.load())
                    {
                        return;
                    }

                    task = std::move(this->_tasks.front());
                    this->_tasks.pop();
                }

                task();
            }
        });
    }
}

THREAD_POOL::~THREAD_POOL()
{
    {
        std::unique_lock<std::mutex> lock(_qmutex);
        _stop.store(true);
    }

    _condition.notify_all();
    for (auto &thread : _threads)
    {
        thread.join();
    }
}
