#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
protected:
    int _numThreads;
    std::vector<std::thread> _threads;
public:
    ThreadPool(int numThreads);
    ~ThreadPool();

    virtual void doWork() = 0;
};

// Thread pool with per thread work queue
// class ThreadPoolParallel : public ThreadPool{
// protected:
//     std::vector<std::queue<std::function<void()>>> _queue;
//     std::vector<std::mutex> _lock;
//     std::vector<std::condition_variable> _cv;
// public:
//     ThreadPoolParallel(int numThreads);
//     void giveJob(int threadId, std::function<void()> job);
//     virtual void doWork() override;
// };

// Thread pool with global work queue
class ThreadPoolQueue : public ThreadPool {
protected:
    std::queue<std::packaged_task<void()>> _queue;
    std::mutex _lock;
    std::condition_variable _cv;
    bool _stop;
public:
    ThreadPoolQueue(int numThreads = std::thread::hardware_concurrency());
    ~ThreadPoolQueue();
    void giveJob(std::packaged_task<void()> job);
    std::future<void> addJob(std::packaged_task<void()> job);
    virtual void doWork() override;
};

#endif