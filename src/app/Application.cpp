
#include "Application.h"
#include "SignalHandler.h"

// #include <csignal>
#include <format>
#include <iostream>

using namespace std;

Application::Application()
    : ioUringLoop_{*this, 1}
{
    SignalHandler::setupSignalHandlers();
    SignalHandler::registerCleanupHandler([this]() {
        this->shutdown();
    });
}

void Application::initialize(){
    
}

void Application::shutdown(){
    cout << "Application is shutting down..." << endl;
    threadPool_.stop();
    ioUringLoop_.stop();
    running_ = false;
}

int add(int a, int b){
    return a + b;
}


int Application::run(){
    cout << "Application is running... "<< endl;
    initialize();
    
    // for (int i = 0; i < 10; ++i) {
    //     threadPool_.enqueue([i]() {
    //         this_thread::sleep_for(chrono::seconds(10));
    //         cout << format(": Task {} is done\n", i) << endl;
    //     });
    // }

    auto future = threadPool_.enqueue(add, 3, 5);
    cout << format("3 + 5 = {}\n", future.get()) << endl;

    if(!ioUringLoop_.start()){
        cerr << "Failed to start IoUringLoop" << endl;
        return EXIT_FAILURE;
    }
    auto handle = ioUringLoop_.listen(8888, [&](int handle){
        cout << "new stream: " << handle << endl;
        // 关闭客户端
        jthread signal_simulator([&, handle]() {
            this_thread::sleep_for(chrono::seconds(50));
            cout << "Simulating SIGINT signal..." << endl;
            // SignalHandler::safe_raise(SIGINT);
            // raise(SIGTERM);
            mainTasks_.enqueue([&, handle](){
                ioUringLoop_.close(handle);
                return;
            });
        signal_simulator.detach();
    });
    });

    if( -1 == handle){
        cerr << "监听端口失败" << endl;
        return EXIT_FAILURE;
    }
    
    // 主线程等待2秒后发送信号（模拟Ctrl+C）
    /*jthread signal_simulator([&, handle]() {
        this_thread::sleep_for(chrono::seconds(5));
        cout << "Simulating SIGINT signal..." << endl;
        // SignalHandler::safe_raise(SIGINT);
        // raise(SIGTERM);
        // 关闭服务器
        mainTasks_.enqueue([&, handle](){
            ioUringLoop_.close(handle);
            return;
        });
    });*/

    while(running_){
        constexpr auto duration = chrono::milliseconds(50);
        auto taskList = mainTasks_.getDoingList(duration);
        if(!taskList->empty()){
            for(auto& task: *taskList){
                task();
            }
            taskList->clear();
        }
    }

    return EXIT_SUCCESS;
}
