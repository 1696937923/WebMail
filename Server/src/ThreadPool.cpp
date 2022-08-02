#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <future>

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
    void addTask(T &&, Args &&...);

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

ThreadPool::ThreadPool(int threads_size)
{
    this->threads_size = threads_size;
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start()
{
    running = true;
    thread_pool.reserve(threads_size);
    for (int i = 0; i < threads_size; i++)
    {
        //为进程绑定threadLoop函数
        std::thread *th = new std::thread(std::bind(&ThreadPool::threadLoop, this));
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
        {
            //执行任务
            task();
        }
    }
}

template <class T, class... Args>
void ThreadPool::addTask(T &&t, Args &&...args)
{
    using RetType = decltype(t(args...)); //构造一个返回类型，参数为传入的args，返回值为T
    auto task = std::make_shared<std::packaged_task<void()>>(
        std::bind(std::forward<T>(t), std::forward<Args>(args)...)); //构造一个task

    // std::future<RetType> future = task->get_future();

    std::unique_lock<std::mutex> lock(m_mutex);
    tasks_queue.emplace([task]()
                        { (*task)(); });

    m_cond.notify_one(); //唤醒一个阻塞的线程
    // return future;
}

Task ThreadPool::take_task()
{
    //进行加锁
    std::unique_lock<std::mutex> lock(m_mutex);
    while (tasks_queue.empty() && running)
    {
        //任务队列为空，则阻塞
        m_cond.wait(lock);
    }

    Task task;
    if (!tasks_queue.empty() && running)
    {
        //从队列中取出一个任务
        task = tasks_queue.front();
        tasks_queue.pop();
    }

    return task;
}

// int MyFunc(int a)
// {
//     // cout << a << endl;
//     std::cout << "PP " << a << " pp" << std::endl;
//     sleep(1);
//     std::cout << "PP " << a << " pp" << std::endl;
//     return 6;
// }

// int main()
// {

//     ThreadPool tp;
//     tp.start();
//     // time_t timer;
//     // clock_t start = time();
//     //这里只添加四次任务，因此任务队列实际大小为4
//     for (int i = 0; i < 5; ++i)
//     {
//         std::future<int> f = tp.addTask(MyFunc, i);
//     }
//     // sleep(1);
//     // clock_t end = clock_t();
//     // cout << end - start << endl;
//     sleep(3);

//     // cout << f.get() << endl;
//     std::cout << "exit(0);" << std::endl;
//     return 0;
// }