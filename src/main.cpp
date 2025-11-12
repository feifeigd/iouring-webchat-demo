#include "thread/ThreadPool.h"
#include <format>
#include <iostream>

using namespace std;

int add(int a, int b){
    return a + b;
}

int main(){
    try
    {
        ThreadPool pool;
        for (int i = 0; i < 10; ++i) {
            pool.enqueue([i]() {
                this_thread::sleep_for(chrono::milliseconds(100));
                cout << format("Task {} is done\n", i) << endl;
            });
        }
        auto future = pool.enqueue(add, 3, 5);
        cout << format("3 + 5 = {}\n", future.get()) << endl;
    }
    catch (const std::exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
    
    
    return 0;
}
