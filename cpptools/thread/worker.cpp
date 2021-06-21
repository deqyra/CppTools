#include "worker.hpp"

namespace cpptools
{

Worker::Worker(std::function<void()> t, bool startNow) :
    _exit(false),
    _execute(startNow),
    _finalized(false),
    _paused(false),
    _running(false),
    _task(std::move(t)),
    _thread(std::async(         // Run the worker right away
        std::launch::async,
        [this]{
            _work();
        }
    ))
{

}

Worker::~Worker()
{
    if (!_exit) finalize();
    _thread.get();
}

void Worker::finalize()
{
    auto l = std::unique_lock<std::mutex>(_mutexExecute);
    _exit = true;
    // Notify thread to resume execution, only to immediately terminate
    _semExecute.notify_all();
}

bool Worker::finalized()
{
    auto l = std::unique_lock<std::mutex>(_mutexFinalized);
    return _finalized;
}

void Worker::waitUntilFinalized(bool finalizeNow)
{
    if (finalizeNow)
    {
        finalize();
    }

    auto l = std::unique_lock<std::mutex>(_mutexFinalized);
    // Wait until the thread says it finalized itself
    _semFinalized.wait(l, [&]{
        return _finalized;
    });
}

void Worker::pause()
{
    auto l = std::unique_lock<std::mutex>(_mutexExecute);
    _execute = false;
    // Don't notify the thread, as the waiting condition is not going
    // away by setting _execute to false.
}

bool Worker::paused()
{
    auto l = std::unique_lock<std::mutex>(_mutexPause);
    return _paused;
}

void Worker::waitUntilPaused(bool pauseNow)
{
    if (pauseNow)
    {
        pause();
    }

    auto l = std::unique_lock<std::mutex>(_mutexPause);
    // Wait until the thread says it paused
    _semPause.wait(l, [&]{
        return _paused;
    });
}

void Worker::run()
{
    auto l = std::unique_lock<std::mutex>(_mutexExecute);
    _execute = true;
    // Notify thread to resume execution
    _semExecute.notify_one();
}

bool Worker::running()
{
    auto l = std::unique_lock<std::mutex>(_mutexRun);
    return _running;
}

void Worker::waitUntilRunning(bool runNow)
{
    if (runNow)
    {
        run();
    }

    auto l = std::unique_lock<std::mutex>(_mutexRun);
    // Wait until the thread says it resumed
    _semRunning.wait(l, [&]{
        return _running;
    });
}

void Worker::_work()
{
    bool stopExecution = false;

    while(true)
    {
        auto l = std::unique_lock<std::mutex>(_mutexExecute);

        // If asked to pause...
        if (!_exit && !_execute)
        {
            // Set the pause flag to true, notify awaiting threads
            // Also set the running flag to false
            {
                auto p = std::unique_lock<std::mutex>(_mutexPause);
                _paused = true;
                _semPause.notify_all();

                auto r = std::unique_lock<std::mutex>(_mutexRun);
                _running = false;
            }

            // Wait for resume signal
            _semExecute.wait(l, [&]{
                return _execute || _exit;
            });

            // Reset pause flag
            {
                auto p = std::unique_lock<std::mutex>(_mutexPause);
                _paused = false;
            }

            // Signify whether to continue or to stop execution.
            stopExecution = _exit;
        }

        if (stopExecution)
        {
            // Exit loop if appropriate
            break;
        }
        else
        {
            // Otherwise notify continuation of thread
            auto r = std::unique_lock<std::mutex>(_mutexRun);
            _running = true;
            _semRunning.notify_all();
        }

        // If continuing execution, do the task
        _task();
    }

    auto f = std::unique_lock<std::mutex>(_mutexFinalized);
    _finalized = true;
    _semFinalized.notify_all();
}

}