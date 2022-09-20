#ifndef CPPTOOLS__THREAD__WORKER_HPP
#define CPPTOOLS__THREAD__WORKER_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

#include "interfaces/interruptible.hpp"

namespace tools::thread
{

class worker : public interruptible
{
public:
    using task_fun = void (*)(void);
private:
    /// @brief Protection around the execution flow flags (_execute and _exit).
    std::mutex _mutex_execute;

    /// @brief Protection around the _finalized flag.
    std::mutex _mutex_finalized;

    /// @brief Protection around the _pause flag.
    std::mutex _mutex_pause;

    /// @brief Protection around the _running flag.
    std::mutex _mutex_run;

    /// @brief Semaphore to wait by in order to control the execution flow.
    std::condition_variable _sem_execute;

    /// @brief Semaphore to wait by in order to know whether the thread
    /// is finalized.
    std::condition_variable _sem_finalized;

    /// @brief Semaphore to wait by in order to know whether the thread
    /// is paused.
    std::condition_variable _sem_pause;

    /// @brief Semaphore to wait by in order to know whether the thread
    /// is running.
    std::condition_variable _sem_running;

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
    task_fun _task;

    /// @brief Function to execute before starting the task in a loop.
    task_fun _on_start;

    /// @brief Function to execute once the task is done running in a loop.
    task_fun _on_finalize;

    /// @brief Function to execute upon resuming after the task paused.
    task_fun _on_resume;

    /// @brief Function to execute upon pause the task.
    task_fun _on_pause;

    /// @brief Handle to the execution thread of the process.
    std::future<void> _thread;

    /// @brief Control the execution of the threaded process using state flags.
    void _work();

public:
    /// @param task Process (function) to execute in a loop.
    /// @param start_now Whether or not to start the process.
    /// @param on_start Function to execute before starting the task in a loop.
    /// @param on_finalize Function to execute once the task is done running.
    /// @param on_resume Function to execute upon resuming after the task paused.
    /// @param on_pause Function to execute upon pause the task.
    worker(
        task_fun task,
        bool start_now = true,
        task_fun on_start = [](){},
        task_fun on_finalize = [](){},
        task_fun on_resume = [](){},
        task_fun on_pause = [](){}
    );
    ~worker();

    // Moving is unsafe. If moving is needed, use unique_ptr<worker>.
    worker(worker&&) = delete;

    /////////////////////////////////////////////
    ///                                       ///
    /// Methods overridden from interruptible ///
    ///                                       ///
    /////////////////////////////////////////////

    virtual void finalize();

    virtual bool finalized();

    virtual void wait_until_finalized(bool finalize_now = true);

    virtual void pause();

    virtual bool paused();

    virtual void wait_until_paused(bool pause_now = true);

    virtual void run();

    virtual bool running();

    virtual void wait_until_running(bool run_now = true);
};

using worker_ptr = std::unique_ptr<worker>;

} // namespace tools::thread

#endif//CPPTOOLS__THREAD__WORKER_HPP