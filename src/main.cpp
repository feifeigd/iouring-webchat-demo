#include "thread/ThreadPool.h"
#include <format>
#include <iostream>

using namespace std;

int main(){
    {
        ThreadPool pool;
        for (int i = 0; i < 10; ++i) {
            pool.enqueue([i]() {
                this_thread::sleep_for(chrono::milliseconds(100));
                cout << format("Task {} is done\n", i) << endl;
            });
        }
    }
    
    return 0;
}
