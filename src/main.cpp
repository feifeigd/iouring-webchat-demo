
#include "app/Application.h"
#include "thread/ThreadPool.h"
#include <iostream>

using namespace std;

int main(){
    
    try
    {
        Application app;
        if(auto rc = app.run(); EXIT_SUCCESS != rc)
        {
            cerr << "Application exited with code: " << rc << endl;
            return rc;
        }
    }
    catch (const std::exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        cerr << "Unknown exception occurred." << endl;
        return EXIT_FAILURE;
    }

    cout << "Main function is exiting." << endl;
    return EXIT_SUCCESS;
}
