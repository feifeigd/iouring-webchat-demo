#pragma once

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

template<typename T, typename Container = std::vector<T>>
class Queue {
    Container twoQueue_[2];
    Container* pendingQueue_;
    Container* doingQueue_;
    std::mutex mutex_;
    std::condition_variable condition;
public:
    Queue(){
        pendingQueue_ = &twoQueue_[0];
        doingQueue_ = &twoQueue_[1];
    }

    void enqueue(T&& item){
        {
            std::lock_guard<std::mutex> lock{mutex_};
            pendingQueue_->push_back(std::move(item));
        }

        condition.notify_one();
    }

    template<typename Rep, typename Period>
    Container* getDoingList(std::chrono::duration<Rep, Period> const& timeout){
        std::unique_lock<std::mutex> lock{mutex_};
        if(doingQueue_->empty()){            
            if(pendingQueue_->empty()){
                bool pendingQueueNotEmpty = condition.wait_for(lock, timeout, [&](){return !pendingQueue_->empty();});
                if(!pendingQueueNotEmpty)
                {
                    assert(pendingQueue_->empty());
                    return doingQueue_;
                }
            }

            std::swap(doingQueue_, pendingQueue_);
        }
        
        return doingQueue_;
    }
};
