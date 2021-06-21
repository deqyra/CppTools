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
    /// @brief Protection around the execution flow flags (_execute and _exit).
    std::mutex _mutexExecute;

    /// @brief Protection around the _finalized flag.
    std::mutex _mutexFinalized;

    /// @brief Protection around the _pause flag.
    std::mutex _mutexPause;

    /// @brief Protection around the _running flag.
    std::mutex _mutexRun;

    /// @brief Semaphore to wait by in order to control the execution flow.
    std::condition_variable _semExecute;

    /// @brief Semaphore to wait by in order to know whether the thread
    /// is finalized.
    std::condition_variable _semFinalized;

    /// @brief Semaphore to wait by in order to know whether the thread
    /// is paused.
    std::condition_variable _semPause;

    /// @brief Semaphore to wait by in order to know whether the thread
    /// is running.
    std::condition_variable _semRunning;

    ///                          ///    \\\ 
    ///                         /// !!!! \\\ 
    ///                        ///        \\\ 
    ///
    /// _exit and _execute are the only flags which control the execution
    /// flow of the threaded process. All other boolean flags are only meant
    /// to be informative.

    /// @brief Stop processing and finalize.
    bool _exit;

    /// @brief Whether to keep executing or to halt.
    bool _execute;

    /// @brief Whether or not the thread is finalized.
    bool _finalized;

    /// @brief Whether or not the thread is paused.
    bool _paused;

    /// @brief Whether or not the thread is running.
    bool _running;

    ///                          ///    \\\ 
    ///                         /// !!!! \\\ 
    ///                        ///        \\\ 
    ///
    /// _exit and _execute are the only flags which control the execution
    /// flow of the threaded process. All other boolean flags are only meant
    /// to be informative.

    /// @brief Process (function) to execute in a loop.
    std::function<void()> _task;

    /// @brief Handle to the execution thread of the process.
    std::future<void> _thread;

    /// @brief Control the execution of the threaded process using state flags.
    void _work();

public:
    /// @param task Process (function) to execute in a loop.
    /// @param startNow Whether or not to start the process.
    Worker(std::function<void()> t, bool startNow = true);
    ~Worker();

    // Moving is unsafe. If moving is needed, use unique_ptr<Worker>.
    Worker(Worker&&) = delete;

    /////////////////////////////////////////////
    ///                                       ///
    /// Methods overridden from Interruptible ///
    ///                                       ///
    /////////////////////////////////////////////

    virtual void finalize();

    virtual bool finalized();

    virtual void waitUntilFinalized(bool finalizeNow = true);

    virtual void pause();
    
    virtual bool paused();

    virtual void waitUntilPaused(bool pauseNow = true);

    virtual void run();

    virtual bool running();

    virtual void waitUntilRunning(bool runNow = true);
};

}//namespace cpptools

#endif//CPPTOOLS__THREAD__WORKER_HPP