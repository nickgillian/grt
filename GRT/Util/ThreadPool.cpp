#include "ThreadPool.h"

using namespace GRT;

//Initalize the static thread pool size to the systems suggested thread limit
std::atomic< unsigned int > ThreadPool::threadPoolSize( std::thread::hardware_concurrency() );

ThreadPool::ThreadPool() : stop(false)
{
    launchThreads( threadPoolSize );
}

ThreadPool::ThreadPool(const unsigned int poolSize) : stop(false)
{
    launchThreads( poolSize );
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

void ThreadPool::launchThreads(const unsigned int poolSize){
    
    for(unsigned int i = 0;i<poolSize; ++i)
        workers.emplace_back(
                             [this]
                             {
                                 for(;;)
                                 {
                                     std::function<void()> task;
                                     
                                     {
                                         std::unique_lock<std::mutex> lock(this->queue_mutex);
                                         this->condition.wait(lock,
                                                              [this]{ return this->stop || !this->tasks.empty(); });
                                         if(this->stop && this->tasks.empty())
                                             return;
                                         task = std::move(this->tasks.front());
                                         this->tasks.pop();
                                     }
                                     
                                     task();
                                 }
                             }
                             );
}

unsigned int ThreadPool::getThreadPoolSize(){
    return threadPoolSize;
}

bool ThreadPool::setThreadPoolSize( const unsigned int threadPoolSize_ ){
    threadPoolSize = threadPoolSize_;
    return true;
}

