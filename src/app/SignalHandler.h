#pragma once

#include <functional>
#include <chrono>
#include <vector>

class SignalHandler {
    using CleanupHandler = std::function<void()>;
    static inline std::vector<CleanupHandler> cleanupHandlers_;
public:
    static void setupSignalHandlers();
    static void registerCleanupHandler(CleanupHandler handler){
        cleanupHandlers_.push_back(std::move(handler));
    }
    
private:
    static void handleSignal(int signal);
};
