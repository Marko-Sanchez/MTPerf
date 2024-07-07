#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <condition_variable>
#include <atomic>
#include <future>
#include <thread>
#include <type_traits>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>
#include <stdexcept>

class THREAD_POOL
{
    private:
        std::mutex _qmutex;
        std::condition_variable _condition;
        std::atomic<bool> _stop;

        std::vector<std::thread> _threads;
        std::queue<std::function<void()>> _tasks;

    public:
        THREAD_POOL(int thread_count);
        ~THREAD_POOL();

        THREAD_POOL(const THREAD_POOL&) = delete;
        THREAD_POOL& operator=(const THREAD_POOL&) = delete;
        THREAD_POOL(THREAD_POOL&&) = delete;
        THREAD_POOL& operator=(THREAD_POOL&&) = delete;

        template<typename F, typename... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>>;
};

template<typename F, typename... Args>
auto THREAD_POOL::enqueue(F&& f, Args&&...args) -> std::future<typename std::invoke_result_t<F, Args...>>
{
    using return_type =  std::invoke_result_t<F, Args...>;

    // create std::shared_ptr to execute later and keep track of its future.
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> ret = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_qmutex);

        if(_stop.load())
        {
            throw std::runtime_error("enqueue on stopped THREAD_POOL");
        }

        // std::shared_ptr is copied into the lambda, lambda is a callable: std::function.
        // that executes the task when lambda is called.
        _tasks.emplace([task](){(*task)();});
    }

    _condition.notify_one();
    return ret;
}

#endif // THREAD_POOL_HPP
