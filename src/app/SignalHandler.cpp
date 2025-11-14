#include "SignalHandler.h"

#include <csignal>
#include <iostream>
#include <system_error>

using namespace std;

void SignalHandler::setupSignalHandlers(){
    struct sigaction sa{};
    sa.sa_handler = SignalHandler::handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // 使系统调用在信号处理后重启

    if(sigaction(SIGINT, &sa, nullptr) == -1){
        throw std::system_error(errno, std::system_category(), "Failed to set SIGINT handler");
    }

    if(sigaction(SIGTERM, &sa, nullptr) == -1){
        throw std::system_error(errno, std::system_category(), "Failed to set SIGTERM handler");
    }

    // 忽略某些可能干扰的信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTRAP, SIG_IGN);
}

void SignalHandler::handleSignal(int signal){    
    cout << "Received signal: " << signal << endl;
    for(auto const& handler : cleanupHandlers_){
        handler();
    }
}
// 安全的信号触发函数
int SignalHandler::safe_raise(int sig) {    
    // 使用kill代替raise，更可控
    return kill(getpid(), sig);
}
