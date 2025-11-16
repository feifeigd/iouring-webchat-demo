
#include "IoUringLoop.h"
#include "IoUringProcessor.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <memory>

using namespace std;

IoUringProcessor::IoUringProcessor(IoUringLoop& loop, uint32_t id): loop_{loop}, id_{id}{

}

IoUringProcessor::~IoUringProcessor(){
    close(pipe_write_);
    pipe_write_ = -1;

    close(pipe_read_);
    pipe_read_ = -1;

    io_uring_queue_exit(&ring_);
}

IoUringProcessor::IoUringProcessor(IoUringProcessor&& other)
    : loop_{other.loop_}, id_{other.id_}, ring_{other.ring_} {
    other.id_ = -1;
    // 注意：io_uring 结构体的移动语义需要根据具体需求实现，这里只是一个简单的示例
    other.ring_ = {};
}

bool IoUringProcessor::init(){
    struct io_uring_params params{};
    if(io_uring_queue_init_params(entries_, &ring_, &params) < 0)
    {
        cerr << "Failed to initialize io_uring for processor " << id_ << endl;
        return {};
    }

    if(-1 == pipe(pipe_fds_)){
        perror("pipe");
        return {};
    }
    if(-1 == fcntl(pipe_write_, F_SETFL, O_NONBLOCK)){
        perror("fcntl pipe_write_");
        return {};
    }
    if(-1 ==fcntl(pipe_read_, F_SETFL, O_NONBLOCK)){
        perror("fcntl pipe_read_");
        return {};
    }

    return true;
}

// 所有的提交，和完成都必须在这个线程处理
void IoUringProcessor::runLoop(){
    // 这里实现 io_uring 事件循环的逻辑
    struct io_uring_cqe* cqe;
    submit_wakeup_read(); 
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
            assert(!cqe);
            cerr << "Error waiting for CQE: " << rc << endl;
        }else{
            // 处理完成的事件
            cout << "Processor " << id_ << " received CQE with user data: " << cqe->user_data << endl;
            bool success = cqe->res >= 0;
            
            if(-1 == cqe->user_data){
                assert(success);
                handle_wakeup_event(); 
                submit_wakeup_read(); 
            }else{
                if(success){
                    handle_io_completion(cqe);
                }else{
                    CommitData& commitData = *(CommitData*)cqe->user_data;
                    cerr << "Async operation failed: " << strerror(-cqe->res) << ", type=" << (uint32_t)commitData.type << endl;
                }
            }

        }

        // 标记 CQE 已处理
        io_uring_cqe_seen(&ring_, cqe);
    }
}


int IoUringProcessor::submit_accept(Listener& listener){
    auto sqe = io_uring_get_sqe(&ring_);
    if(!sqe){
        return -1;
    }

    io_uring_prep_accept(sqe, listener.fd(), (struct sockaddr*)&listener.client_addr, &listener.client_len, 0);
    io_uring_sqe_set_data(sqe, &listener.commitData_);
    return io_uring_submit(&ring_);
}

void IoUringProcessor::removeListener(int handle){
    auto it = listeners_.find(handle);
    if(listeners_.end() != it){
        it->second.pendingClose_ = true;
        it->second.close();
    }
}

void IoUringProcessor::removeStream(int handle){
    auto& stream = streams_.at(handle);
    stream.close();
}

void IoUringProcessor::addListener(int handle, Listener&& listener){
    std::lock_guard<std::mutex> lock{mutex_};
    pending_listeners_.emplace(handle, std::move(listener));
    wakeup_io_uring(NEW_LISTENER);
}

void IoUringProcessor::addStream(int handle, Stream&& stream){
    streams_.emplace(handle, std::move(stream));
    submit_read(streams_.at(handle));
}

int IoUringProcessor::submit_read(Stream& stream){
    auto sqe = io_uring_get_sqe(&ring_);
    if(!sqe){
        return -1;
    }

    io_uring_prep_read(sqe, stream.fd(), stream.getReadBuff(), stream.getReadBuffSize(), 0);
    io_uring_sqe_set_data(sqe, &stream.getCommitDataRead());
    return io_uring_submit(&ring_);
}

int IoUringProcessor::submit_write(int handle, Stream& stream){
    auto sqe = io_uring_get_sqe(&ring_);
    if(!sqe){
        return -1;
    }

    io_uring_prep_write(sqe, stream.fd(), stream.getWriteBuff(), stream.getWriteSize(), 0);
    io_uring_sqe_set_data(sqe, &stream.getCommitDataWrite());
    return io_uring_submit(&ring_);
}

void IoUringProcessor::submit_wakeup_read(){
    auto sqe = io_uring_get_sqe(&ring_);
    if(!sqe){
        cerr << "submit_wait_wakeup sqe is null" << endl;
        // TODO
        return;
    }
    
    io_uring_prep_read(sqe, pipe_read_, &wakeupData_, sizeof(wakeupData_), 0);
    io_uring_sqe_set_data64(sqe, -1);
    io_uring_submit(&ring_);
}

void IoUringProcessor::wakeup_io_uring(WakeupType wakeup_type, uint64_t param){
    WakeupData wakeupData{
        .type = wakeup_type,
        .param = param,
    };
    constexpr auto size{sizeof(wakeupData)};
    if(auto res = write(pipe_write_, &wakeupData, sizeof(wakeupData)); res != size){
        auto msg = std::format("write to eventfd={}", pipe_write_);
        perror(msg.c_str());
    }
}

void IoUringProcessor::handle_wakeup_event(){
    cout << "wakeup_type=" << wakeupData_.type << endl;
    switch ((WakeupType)wakeupData_.type)
    {    
    case NEW_LISTENER:{
        std::lock_guard<std::mutex> lock{mutex_};
        for(auto& [handle, listener]: pending_listeners_){
            listeners_.emplace(handle, std::move(listener));
            submit_accept(listeners_.at(handle));
        }
        pending_listeners_.clear();
    }
        break;
    default:
        assert(false);
        break;
    }
}

void IoUringProcessor::handle_io_completion(struct io_uring_cqe* cqe){
    CommitData& commitData = *(CommitData*)cqe->user_data;
    
    switch (commitData.type)
    {
    case CommitType::ACCEPT:{
        auto& listener = *dynamic_cast<Listener*>(commitData.netItem);
        loop_.createStream(cqe->res, listener.onNewClient_);
        submit_accept(listener);
    }
    break;

    default: {
        auto& stream = *dynamic_cast<Stream*>(commitData.netItem);
        auto handle = stream.handle();
        auto bytes = cqe->res;
        switch (commitData.type)
        {
        case CommitType::READ:{
            auto buff = stream.getReadBuff();
            buff[bytes] = 0;
            cout << buff << endl;
            if(bytes > 0){
                // echo
                stream.write(buff, bytes);
                submit_write(handle, stream);
                submit_read(stream);
            }else{
                cout << "客户端已断开 handle= " << handle << endl;
            }
        }
            
        break;

        case CommitType::WRITE:
            cout << "发送完成: " << bytes << endl;
            stream.resetWriteBuff();
        break;
        default:
            assert(false);
        break;
        }
        break;
    }
    break;
    }
}
