/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 
 @brief The ThreadPool class implements a flexible inteface for performing a large number of batch tasks. You need to build the GRT with
 GRT_CXX11_ENABLED, otherwise the ThreadPool class will be empty (as it requires C++11 support).
 
 @note This class is mainly based on the following thread pool example: https://github.com/progschj/ThreadPool/
 */

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
*/

#ifndef GRT_THREAD_POOL_HEADER
#define GRT_THREAD_POOL_HEADER

//Include the common C++ headers
#include <vector>
#include <queue>
#include <stdexcept>

#ifdef GRT_CXX11_ENABLED
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#endif //GRT_CXX11_ENABLED

#include "GRTTypedefs.h"

GRT_BEGIN_NAMESPACE
    
class GRT_API ThreadPool {
public:
    
    /**
     Default Constructor. If this constructor is used, the number of threads will automatically be set to the current thread pool size.
     */
    ThreadPool();
    
    /**
     If this constructor is used, the number of threads will be set to the numThreads value.
     
     @param const unsigned int numThreads: sets the number of threads in the thread pool.
     */
    ThreadPool(const unsigned int numThreads);
    
    /**
     Default Destructor. Waits for all threads to finish their current tasks.
     */
    ~ThreadPool();

#ifdef GRT_CXX11_ENABLED
    /**
     This function should be used to add new jobs to the thread pool.  The function enables the user to pass in a reference function (the task) and additional
     arguments for that function (if needed).  The function will return a future variable with a type specified by the return type of the task function (F).

     @note This function will only be enabled if the GRT is compiled with C++11 support.
     
     @param f: a reference to the function you want to queue
     @param args: one or more arguments for the function (f)
     @param future< T >: a future variable that will store the result of the function (f), the type (T) will be specified by the return type of the function (f)
     */
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future< typename std::result_of<F(Args...) >::type>;
#endif //GRT_CXX11_ENABLED

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
#ifdef GRT_CXX11_ENABLED
    void launchThreads(const unsigned int threads);
    
    std::vector< std::thread > workers;
    std::queue< std::function<void()> > tasks;
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic< bool > stop;
    
    static std::atomic< unsigned int > threadPoolSize;
#endif //GRT_CXX11_ENABLED
};
    
#ifdef GRT_CXX11_ENABLED
// This function adds a new work item (func) to the thread pool
template<class F, class... Args> auto ThreadPool::enqueue(F&& func, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    //Get a shortcut to the return type of the input function
    using return_type = typename std::result_of< F(Args...) >::type;
    
    auto task = std::make_shared< std::packaged_task< return_type() > >( std::bind(std::forward<F>( func ), std::forward<Args>(args)...) );
    
    std::future< return_type > res = task->get_future();
    {
        std::unique_lock< std::mutex > lock( queue_mutex );
        
        // don't allow enqueueing after stopping the pool
        if( stop )
            throw std::runtime_error("enqueue on stopped ThreadPool");
        
        tasks.emplace( [task](){ (*task)(); } );
    }
    condition.notify_one();
    return res;
}
#endif //GRT_CXX11_ENABLED
    
GRT_END_NAMESPACE

#endif //GRT_THREAD_POOL_HEADER
