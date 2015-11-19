#include "ThreadPool.h"

using namespace GRT;

#ifdef GRT_CXX11_ENABLED
//Initalize the static thread pool size to the systems suggested thread limit
std::atomic< unsigned int > ThreadPool::threadPoolSize( std::thread::hardware_concurrency() );
#endif

ThreadPool::ThreadPool()
{
#ifdef GRT_CXX11_ENABLED
    stop = false;
    launchThreads( threadPoolSize );
#endif
}

ThreadPool::ThreadPool(const unsigned int poolSize)
{
#ifdef GRT_CXX11_ENABLED
    stop = false;
    launchThreads( poolSize );
#endif
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
#ifdef GRT_CXX11_ENABLED
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
#endif
}

#ifdef GRT_CXX11_ENABLED
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
                                         this->condition.wait(lock,[this]{ return this->stop || !this->tasks.empty(); });
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
#endif

unsigned int ThreadPool::getThreadPoolSize(){
#ifdef GRT_CXX11_ENABLED
    return threadPoolSize;
#else
    return 0;
#endif
}

bool ThreadPool::setThreadPoolSize( const unsigned int threadPoolSize_ ){
#ifdef GRT_CXX11_ENABLED
    threadPoolSize = threadPoolSize_;
    return true;
#else
    return false;
#endif
}

