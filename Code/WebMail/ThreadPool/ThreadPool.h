#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <future>
#include <stdexcept>

typedef std::function<void()> Task;

class ThreadPool
{
public:
    //默认线程数为5
    ThreadPool(int threads_size = 5);
    ~ThreadPool();

    //开启线程池
    void start();
    void stop();

    //向线程池中增加task
    template <class T, class... Args>
    auto addTask(T &&, Args &&...);

private:
    void threadLoop();
    Task take_task();
    int threads_size;
    bool running;

    //锁
    std::mutex m_mutex;

    //条件变量
    std::condition_variable m_cond;

    //线程池
    std::vector<std::thread *> thread_pool;

    //任务队列
    std::queue<Task> tasks_queue;
};