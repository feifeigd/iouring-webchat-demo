#include "IoUringLoop.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <climits>

int IoUringLoop::nextHandle_;

IoUringLoop::IoUringLoop(uint32_t numThreads) : numThreads_(numThreads) {
    if(numThreads_ <= 0) {
        numThreads_ = 1;
    }
    if(numThreads_ & (numThreads_ - 1) ){
        throw std::runtime_error("numThreads 必须是2的N次幂");
    }
    index_mask_ = numThreads_ - 1;
    processors_.reserve(numThreads);
    for(uint32_t id = 0; id < numThreads; ++id){
        processors_.emplace_back(id);
    }
}

IoUringLoop::~IoUringLoop() {
    stop();
}

int IoUringLoop::generateHandle() {
    auto res = nextHandle_;
    if(std::numeric_limits<int>::max() == ++nextHandle_){
        nextHandle_ = 0;
    }
    return res;
}

bool IoUringLoop::start() {
    // 启动 io_uring 事件循环
    for(auto& processor : processors_){
        if(!processor.init()){
            return false;
        }
        threads_.emplace_back([&processor]() {
            processor.runLoop();
            // 这里可以添加事件循环的逻辑
        });
    }

    return true;
}

void IoUringLoop::stop() {
    // 停止 io_uring 事件循环，清理资源
}

int IoUringLoop::listen(uint16_t port, Listener::OnNewClient onNewClient){
    auto fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(fd < 0){
        perror("socket");
        return -1;
    }

    const int enable = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0){
        perror("setsockopt SO_REUSEADDR");
        close(fd);
        return -1;        
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind");
        close(fd);
        return -1;        
    }

    if(::listen(fd, 512) < 0){
        perror("listen");
        close(fd);
        return -1;        
    }

    auto handle = generateHandle() | 0x80000000;
    getProcessor(handle).createListener(handle, fd, port, std::move(onNewClient));
    return handle;
}

IoUringProcessor& IoUringLoop::getProcessor(int handle){
    auto index = handle & index_mask_;
    return processors_[index];
}

void IoUringLoop::close(int handle){
    auto& processor = getProcessor(handle);
    if(0x7fffffff & handle){
        processor.removeListener(handle);
    }else{
        processor.removeStream(handle);
    }
}
