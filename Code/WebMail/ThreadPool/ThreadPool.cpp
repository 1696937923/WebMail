#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
using namespace std;
typedef function<void()> Task;

class ThreadPool
{
public:
    ThreadPool(int threads_size = 5);
    ~ThreadPool();
    void stop();
    void addTask(Task);

private:
    void start();
    void threadLoop();
    Task take_task();
    int threads_size;
    mutex m_mutex;
    condition_variable m_cond; //条件变量
    bool m_started;            //是否开始

    vector<thread *> thread_pool; //线程池
    queue<Task> tasks_queue;      //任务队列
};

ThreadPool::ThreadPool(int threads_size)
{
    this->threads_size = threads_size;
    start();
}

ThreadPool::~ThreadPool()
{
    if (m_started)
    {
        stop();
    }
}

void ThreadPool::start()
{
    m_started = true;
    thread_pool.reserve(threads_size);
    for (int i = 0; i < threads_size; i++)
    {
        //为进程绑定threadLoop函数
        thread *th = new thread(bind(&ThreadPool::threadLoop, this));
        thread_pool.push_back(th);
    }
}

//线程池终止
void ThreadPool::stop()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_started = false;
    m_cond.notify_all();

    for (auto it = thread_pool.begin(); it != thread_pool.end(); ++it)
    {
        (*it)->join();
        delete *it;
    }
    thread_pool.clear();
}

//线程池循环
void ThreadPool::threadLoop()
{
    while (true)
    {
        Task task = take_task();
        if (task)
            task();
    }
}

void ThreadPool::addTask(Task task)
{
    unique_lock<mutex> lock(m_mutex);
    tasks_queue.push(task);
    m_cond.notify_one();
}

Task ThreadPool::take_task()
{
    // cout << "1" << endl;
    unique_lock<mutex> lock(m_mutex);

    while (tasks_queue.empty())
    {
        cout << this_thread::get_id() << endl;
        m_cond.wait(lock);
    }
    cout << 3 << endl;
    Task task;
    if (!tasks_queue.empty() && m_started)
    {
        task = tasks_queue.front();
        tasks_queue.pop();
    }
    return task;
}

void MyFunc()
{
    cout << " at thread[" << this_thread::get_id() << "] output " << endl;
}

int main()
{
    ThreadPool tp;
    //这里只添加四次任务，因此任务队列实际大小为4
    // for (int i = 0; i < 4; ++i)
    // {
    //     tp.addTask(MyFunc);
    // }
    sleep(10);
    cout << "exit(0);" << endl;
    return 0;
}