#include "IoUringProcessor.h"

#include <cstring>
#include <iostream>
#include <memory>

using namespace std;

IoUringProcessor::IoUringProcessor(uint32_t id): id_{id}{

}

IoUringProcessor::~IoUringProcessor(){
    io_uring_queue_exit(&ring_);
}

IoUringProcessor::IoUringProcessor(IoUringProcessor&& other)
    : id_{other.id_}, ring_{other.ring_} {
    other.id_ = -1;
    // 注意：io_uring 结构体的移动语义需要根据具体需求实现，这里只是一个简单的示例
    other.ring_ = {};
}

bool IoUringProcessor::init(){
    // struct io_uring_params params{};
    // if(io_uring_queue_init_params(entries_, &ring_, &params) < 0)
    if(io_uring_queue_init(entries_, &ring_, 0) < 0)
    {
        cerr << "Failed to initialize io_uring for processor " << id_ << endl;
        return {};
    }
    return true;
}

void IoUringProcessor::runLoop(){
    // 这里实现 io_uring 事件循环的逻辑
    struct io_uring_cqe* cqe;
    while(true){
        // 处理事件
        if(auto rc = io_uring_wait_cqe(&ring_, &cqe))
        {
            if(-EINTR == rc){
                #ifdef DEBUG
                // 断点调试会触发 -4 返回值
                continue;
                #endif
            }
            cerr << "Error waiting for CQE: " << rc << endl;
        }else{
            // 处理完成的事件
            cout << "Processor " << id_ << " received CQE with user data: " << cqe->user_data << endl;
            bool success = cqe->res >= 0;
            if(success){
                int handle = (int)cqe->user_data;
                if(0x80000000 & handle){
                    auto& listener = listeners_.at(handle);

                    submit_accept(handle, listener);
                }else{
                    auto& stream = streams_.at(handle);
                }
            }else{
                cerr << "Async operation failed: " << strerror(-cqe->res) << endl;
            }
        }

        // 标记 CQE 已处理
        io_uring_cqe_seen(&ring_, cqe);
    }
}

void IoUringProcessor::createListener(int handle, int fd, uint16_t port, Listener::OnNewClient onNewClient){
    listeners_.emplace(handle, Listener{fd, port, std::move(onNewClient)});
    auto& listener = listeners_.at(handle);
    auto res = submit_accept(handle, listener);
}

int IoUringProcessor::submit_accept(int handle, Listener& listener){
    auto sqe = io_uring_get_sqe(&ring_);
    if(!sqe){
        return -1;
    }

    io_uring_prep_accept(sqe, listener.fd(), (struct sockaddr*)&listener.client_addr, &listener.client_len, 0);
    io_uring_sqe_set_data(sqe, (void*)(unsigned int)handle);
    return io_uring_submit(&ring_);
}

void IoUringProcessor::removeListener(int handle){
    auto it = listeners_.find(handle);
    if(listeners_.end() != it){
        it->second.pendingClose_ = true;
    }
}

void IoUringProcessor::removeStream(int handle){

}
