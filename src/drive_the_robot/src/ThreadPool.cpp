#include "ThreadPool.h"
#include <thread>
#include <iostream>

ThreadPool::ThreadPool(int numThreads) : _numThreads(numThreads){

}

ThreadPool::~ThreadPool(){ }


// ThreadPoolParallel::ThreadPoolParallel(int numThreads = std::thread::hardware_concurrency()) : ThreadPool(numThreads) {
//     for(int i = 0; i < numThreads; i++){
//         _queue.push_back(std::queue<std::function<void()>>());
//     }
// }

// void ThreadPoolParallel::giveJob(int threadId, std::function<void()> job){
// }

// void ThreadPoolParallel::doWork(){

// }

ThreadPoolQueue::ThreadPoolQueue(int numThreads) : ThreadPool(numThreads), _stop(false) {
    for(int i = 0; i < numThreads; i++){
        // TODO figure out what's wrong with virtual method here (i.e. should be in base class)
        _threads.push_back(std::thread(&ThreadPool::doWork, this)); 
    }
}

ThreadPoolQueue::~ThreadPoolQueue(){
    std::unique_lock<std::mutex> lock(_lock);
    if(!_queue.empty()){
        std::cout << "Warning: destructing thread pool with " << _queue.size() << " jobs remaining" << std::endl;
    }
    _stop = true;
    lock.unlock();

    _cv.notify_all();
    for(int i = 0; i < _numThreads; i++){
        _threads[i].join();
    }
}

/**
 * @brief Add task to job queue
 * 
 * @param job 
 */
void ThreadPoolQueue::giveJob(std::packaged_task<void()> job){
    {
        std::lock_guard<std::mutex> lock(_lock);
        _queue.push(std::move(job));
    }
    _cv.notify_one();
}

/**
 * @brief Add task to job queue
 * 
 * @param job 
 * @return std::future<void> Future to wait on results
 */
std::future<void> ThreadPoolQueue::addJob(std::packaged_task<void()> job){
    auto ret = job.get_future();
    giveJob(std::move(job));
    return ret;
}

void ThreadPoolQueue::doWork(){
    while(true){
        // Wait on queue update or termination
        std::unique_lock<std::mutex> lock(_lock);
        _cv.wait(lock, [this]{ return !_queue.empty() || _stop; });
        if(_stop){
            return;
        }

        // Get job from queue
        auto job = std::move(_queue.front());
        _queue.pop();        
        lock.unlock();

        // Execute work
        job();
    }
}