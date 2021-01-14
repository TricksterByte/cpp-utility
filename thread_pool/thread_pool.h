#pragma once

#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

const std::size_t max_num_threads{std::thread::hardware_concurrency()};

class thread_pool
{
public:

    thread_pool(std::size_t numthreads = max_num_threads);
    ~thread_pool();

    // std::thread instances are non-copyable,
    // so it doesn't make sense for thread_pool to be copyable
    thread_pool(const thread_pool& other) = delete;
    thread_pool& operator=(const thread_pool& other) = delete;

    template<typename F, typename... Args,
    std::enable_if_t<std::is_invocable_v<F&&, Args&&...>, bool> = true>
    auto execute(F&& function, Args&&... args);

private:

    struct task_base
    {
    public:
        virtual ~task_base() = default;

        virtual void operator()() = 0;
    };

    template<typename F, 
    std::enable_if_t<std::is_invocable_v<F&&>, bool> = true>
    struct task : public task_base
    {
    public:
        explicit task(F &&f) : func(std::forward<F>(f)) {}

        void operator()() override
        {
            func();
        }

    private:
        F func;
    };

    using task_ptr = std::unique_ptr<task_base>;
    template<typename F> task(F) -> task<std::decay<F>>;

    std::vector<std::thread> _threads;
    std::queue<task_ptr> _tasks;
    std::mutex _task_mutex;
    std::condition_variable _cv;
    bool _stop_threads = false;
};

template<typename F, typename... Args,
std::enable_if_t<std::is_invocable_v<F&&, Args&&...>, bool>>
auto thread_pool::execute(F&& function, Args&&... args)
{
    std::unique_lock queue_lock(_task_mutex, std::defer_lock);
    std::packaged_task<std::invoke_result_t<F, Args...>()> task_pkg(
        [f = std::move(function),
        fargs = std::forward_as_tuple(args...)]() mutable {
            return std::apply(std::move(f), std::move(fargs));
        }
    );

    std::future<std::invoke_result_t<F, Args...>> future = task_pkg.get_future();

    queue_lock.lock();

    _tasks.emplace(task_ptr(
        new task([task(std::move(task_pkg))]() mutable { task(); })));

    queue_lock.unlock();

    _cv.notify_one();

    return future;
}
