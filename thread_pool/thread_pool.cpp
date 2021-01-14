#include "thread_pool.h"

thread_pool::thread_pool(std::size_t numthreads)
{
    for (unsigned int ix = 0; ix < numthreads; ++ix)
    {
        _threads.emplace_back(std::thread([&]() {
            std::unique_lock<std::mutex> queue_lock(_task_mutex, std::defer_lock);
            
            while (true)
            {
                queue_lock.lock();
                _cv.wait(queue_lock, [&]() -> bool {
                    return !_tasks.empty() || _stop_threads;
                });

                if (_tasks.empty() && _stop_threads)
                    return;

                auto temp = std::move(_tasks.front());
                _tasks.pop();
                queue_lock.unlock();

                (*temp)();
            }
        }));
    }
}

thread_pool::~thread_pool()
{
    _stop_threads = true;
    _cv.notify_all();

    for (std::thread& thread : _threads)
        thread.join();
}
