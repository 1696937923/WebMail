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

using namespace std;
typedef function<void()> Task;

class ThreadPool
{
public:
    ThreadPool(int threads_size = 5);
    ~ThreadPool();
    void start();
    void stop();

    template <class T, class... Args>
    void addTask(T &&, Args &&...);

private:
    void threadLoop();
    Task take_task();
    int threads_size;
    bool running;
    mutex m_mutex;
    condition_variable m_cond; //条件变量

    vector<thread *> thread_pool; //线程池
    queue<Task> tasks_queue;      //任务队列
};

ThreadPool::ThreadPool(int threads_size)
{
    this->threads_size = threads_size;
    running = true;
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start()
{
    thread_pool.reserve(threads_size);
    for (int i = 0; i < threads_size; i++)
    {
        //为进程绑定threadLoop函数
        thread *th = new thread(bind(&ThreadPool::threadLoop, this));
        thread_pool.push_back(th);
    }
}

void ThreadPool::stop()
{
    running = false;
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.notify_all();

    for (auto it = thread_pool.begin(); it != thread_pool.end(); ++it)
    {
        (*it)->join();
        delete *it;
    }
    thread_pool.clear();
}

void ThreadPool::threadLoop()
{
    while (running)
    {
        Task task = take_task();
        if (task)
            task();
    }
}

template <class T, class... Args>
void ThreadPool::addTask(T &&t, Args &&...args)
{
    // using RetType = decltype(t(args...));
    auto task = std::make_shared<std::packaged_task<int()>>(
        std::bind(std::forward<T>(t), std::forward<Args>(args)...));

    unique_lock<mutex> lock(m_mutex);
    tasks_queue.emplace([task]()
                        { (*task)(); });
    m_cond.notify_one();
}

Task ThreadPool::take_task()
{
    unique_lock<mutex> lock(m_mutex);
    while (tasks_queue.empty() && running)
    {
        m_cond.wait(lock);
    }
    Task task;
    if (!tasks_queue.empty() && running)
    {
        task = tasks_queue.front();
        tasks_queue.pop();
    }
    return task;
}

// void MyFunc(int a, int b)
// {
//     // cout << a << endl;
//     // cout << " at thread[" << this_thread::get_id() << "] output " << endl;
// }

// int main()
// {
//     ThreadPool tp;
//     tp.start();
//     //这里只添加四次任务，因此任务队列实际大小为4
//     for (int i = 0; i < 4; ++i)
//     {
//         tp.addTask(&MyFunc, 3, i);
//     }
//     sleep(3);
//     // tp.addTask(MyFunc);
//     sleep(3);
//     cout << "exit(0);" << endl;
//     return 0;
// }