#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>   // for std::packaged_task
#include <memory>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
    using Task = std::function<void()>;
    using TaskPtr = std::unique_ptr<Task>;
public:

    ThreadPool(uint32_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args){
        using ReturnType = std::invoke_result_t<F, Args...>;
        constexpr bool returns_void = std::is_void_v<ReturnType>;
        if constexpr(returns_void){
            auto task = std::make_unique<Task>(std::forward<F>(f), std::forward<Args>(args)...);
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if(stop_){
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                }
                tasks.push(std::move(task));
            }
            condition.notify_one();
        }else{
            auto taskPtr = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
            auto resPtr = taskPtr->get_future();
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if(stop_){
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                }
                tasks.emplace(std::make_unique<Task>([taskPtr = std::move(taskPtr)]() {
                    (*taskPtr)();
                }));
            }
            condition.notify_one();
            return resPtr;
        }
    }

    bool empty()const;
    void stop();
private:
    void workerThread();
    std::queue<TaskPtr> tasks; // 必须放在 workers 之前，保证析构时先停止线程再销毁任务队列
    mutable std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop_;
    std::vector<std::jthread> workers;
};
