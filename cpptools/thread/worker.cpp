#include "worker.hpp"

namespace cpptools
{

Worker::Worker(std::function<void()> t, bool startNow) :
    _task(std::move(t)),
    _exit(false),
    _execute(startNow),
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
    if (!exit) finalize();
    _thread.get();
}

void Worker::finalize()
{
    auto l = _lock();
    _exit = true;
    _cond.notify_one();
}

void Worker::pause()
{
    auto l = _lock();
    _execute = false;
}

void Worker::resume()
{
    auto l = _lock();
    _execute = true;
    _cond.notify_one();
}

void Worker::_work()
{
    while(true)
    {
        bool stopExecution = false;
        {
            auto l = _lock();

            // In the general case, _exit will be false and _execute will be 
            // true, so the execution flow will fall right through the condition
            // variable. When _execute becomes false, the execution will halt 
            // either until it goes back to true, or until asked to stop 
            // execution entirely, when _exit becomes true.
            _cond.wait(l, [&]{
                    return _exit || _execute;
            });

            // Signify whether to continue or to stop execution.
            stopExecution = _exit;
        }

        if (stopExecution)
        {
            break;
        }

        // If continuing execution, do the task.
        _task();
    }
}

std::unique_lock<std::mutex> Worker::_lock()
{
    return std::unique_lock<std::mutex>(_mutex);
}

}