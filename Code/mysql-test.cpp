#include <iostream>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
using namespace std;
std::mutex mymutex;
condition_variable m_cond;

void sayHello()
{
    // unique_lock<mutex> lock(mymutex);
    int i = 0;
    while (1)
    {
        i++;
        cout << "hello ";
        // if (i == 10)
        //     m_cond.wait(lock);
        sleep(0.5);
    }
}
void sayWorld()
{
    // unique_lock<mutex> lock(mymutex);
    int i = 0;
    while (1)
    {
        i++;
        cout << "world ";
        // if (i == 10)
        // {
        //     m_cond.notify_one();
        //     m_cond.wait(lock);
        // }
        sleep(0.5);
    }
}
int main()
{
    thread threadHello(&sayHello);
    sleep(0.5);
    thread threadWorld(&sayWorld);
    threadHello.join();
    threadWorld.join();
    // sleep(10);
    return 0;
}