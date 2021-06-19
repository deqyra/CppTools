#ifndef CPPTOOLS__THREAD__WORKER_HPP
#define CPPTOOLS__THREAD__WORKER_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <thread>

#include "interfaces/interruptible.hpp"

namespace cpptools
{

class Worker : public Interruptible
{
private:
    /// @brief Protection around the state flags.
    std::mutex _mutex;

    /// @brief Conditional to check upon in order to know whether to keep 
    /// processing or to halt.
    std::condition_variable _cond;

    /// @brief Stop processing entirely.
    bool _exit;

    /// @brief Whether to execute or to halt.
    bool _execute;

    /// @brief Function to execute in a loop.
    std::function<void()> _task;

    /// @brief Handled to the execution thread of the function.
    std::future<void> _thread;

    /// @brief Execute the function in a loop until asked to stop.
    void _work();

    /// @brief Lock and acquire the mutex around state flags.
    std::unique_lock<std::mutex> _lock();

public:
    /// @param task Function to execute in a loop.
    /// @param startNow Whether or not to start the function.
    Worker(std::function<void()> t, bool startNow = true);
    ~Worker();

    // Moving is unsafe. If moving is needed, use unique_ptr<Worker>.
    Worker(Worker&&) = delete;

    /////////////////////////////////////////////
    ///                                       ///
    /// Methods overridden from Interruptible ///
    ///                                       ///
    /////////////////////////////////////////////

    /// @brief Ask for the threaded function to stop processing and cleanly exit.
    virtual void finalize();

    /// @brief Pause the execution of the threaded function.
    virtual void pause();
    
    /// @brief Resume the execution of the threaded function.
    virtual void resume();
};

}//namespace cpptools

#endif//CPPTOOLS__THREAD__WORKER_HPP