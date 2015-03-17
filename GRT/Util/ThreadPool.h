/*
GRT MIT License
Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 This class is mainly based on the following thread pool example: https://github.com/progschj/ThreadPool/
*/

#ifndef GRT_THREAD_POOL_HEADER
#define GRT_THREAD_POOL_HEADER

//Include the common C++ headers
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace GRT{
    
class ThreadPool {
public:
    
    ThreadPool();
    
    ThreadPool(const unsigned int numThreads);
    
    ~ThreadPool();
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    
    /**
     This function returns the current thread limit.  This defaults to the number of threads set by std::thread::hardware_concurrency(), but the user
     can override this value if needed using the setThreadLimit(...) function.
     
     @return returns the number of concurrent threads either (i) supported by the implementation, or (ii) set by the user.
     */
    static unsigned int getThreadPoolSize();
    
    /**
     This function sets the current thread limit.  This defaults to the number of threads set by std::thread::hardware_concurrency(), but the user
     can override this value using this function.
     
     @return returns ture if the value was updated, false otherwise
     */
    static bool setThreadPoolSize( const unsigned int threadPoolSize );
    
protected:
    void launchThreads(const unsigned int threads);
    
    std::vector< std::thread > workers;
    std::queue< std::function<void()> > tasks;
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
    
    static std::atomic< unsigned int > threadPoolSize;
};
    
// add new work item to the pool
template<class F, class... Args> auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared< std::packaged_task<return_type()> >( std::bind(std::forward<F>(f), std::forward<Args>(args)...) );
    
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        
        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}
    
}

#endif //GRT_THREAD_POOL_HEADER
