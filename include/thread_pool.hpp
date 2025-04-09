#define pragma once

#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <functional>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Enqueue a task into the pool.
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
         -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    // Workers in the pool.
    std::vector<std::thread> workers;
    // Task queue.
    std::queue<std::function<void()>> tasks;
    
    // Synchronization.
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

