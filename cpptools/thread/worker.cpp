#include "worker.hpp"

namespace tools::thread
{

worker::worker(
    task_fun task,
    bool start_now,
    task_fun on_start,
    task_fun on_finalize,
    task_fun on_resume,
    task_fun on_pause
) :
    _exit(false),
    _execute(start_now),
    _finalized(false),
    _paused(false),
    _running(false),
    _task(std::move(task)),
    _on_start(std::move(on_start)),
    _on_finalize(std::move(on_finalize)),
    _on_resume(std::move(on_resume)),
    _on_pause(std::move(on_pause)),
    _thread(std::async(         // Run the worker right away
        std::launch::async,
        [this]{
            _work();
        }
    ))
{

}

worker::~worker()
{
    if (!_exit) finalize();
    _thread.get();
}

void worker::finalize()
{
    auto l = std::unique_lock<std::mutex>(_mutex_execute);
    _exit = true;
    // Notify thread to resume execution, only to immediately terminate
    _sem_execute.notify_all();
}

bool worker::finalized()
{
    auto l = std::unique_lock<std::mutex>(_mutex_finalized);
    return _finalized;
}

void worker::wait_until_finalized(bool finalize_now)
{
    if (finalize_now)
    {
        finalize();
    }

    auto l = std::unique_lock<std::mutex>(_mutex_finalized);
    // Wait until the thread says it finalized itself
    _sem_finalized.wait(l, [&]{
        return _finalized;
    });
}

void worker::pause()
{
    auto l = std::unique_lock<std::mutex>(_mutex_execute);
    _execute = false;
    // Don't notify the thread, as the waiting condition is not going
    // away by setting _execute to false.
}

bool worker::paused()
{
    auto l = std::unique_lock<std::mutex>(_mutex_pause);
    return _paused;
}

void worker::wait_until_paused(bool pause_now)
{
    if (pause_now)
    {
        pause();
    }

    auto l = std::unique_lock<std::mutex>(_mutex_pause);
    // Wait until the thread says it paused
    _sem_pause.wait(l, [&]{
        return _paused;
    });
}

void worker::run()
{
    auto l = std::unique_lock<std::mutex>(_mutex_execute);
    _execute = true;
    // Notify thread to resume execution
    _sem_execute.notify_one();
}

bool worker::running()
{
    auto l = std::unique_lock<std::mutex>(_mutex_run);
    return _running;
}

void worker::wait_until_running(bool run_now)
{
    if (run_now)
    {
        run();
    }

    auto l = std::unique_lock<std::mutex>(_mutex_run);
    // Wait until the thread says it resumed
    _sem_running.wait(l, [&]{
        return _running;
    });
}

void worker::_work()
{
    bool stop_execution = false;
    bool was_running_before = false;

    _on_start();
    while(true)
    {
        auto l = std::unique_lock<std::mutex>(_mutex_execute);

        // If asked to pause...
        if (!_exit && !_execute)
        {
            // Set the pause flag to true, notify awaiting threads
            // Also set the running flag to false
            {
                auto p = std::unique_lock<std::mutex>(_mutex_pause);
                _paused = true;
                _sem_pause.notify_all();
                p.unlock();

                auto r = std::unique_lock<std::mutex>(_mutex_run);
                _running = false;
            }

            _on_pause();

            // Wait for resume signal
            _sem_execute.wait(l, [&]{
                return _execute || _exit;
            });

            // Signify whether to continue or to stop execution.
            stop_execution = _exit;
            l.unlock();

            // Reset pause flag
            {
                auto p = std::unique_lock<std::mutex>(_mutex_pause);
                _paused = false;
            }
            was_running_before = false;
        }
        else if (_exit)
        {
            stop_execution = _exit;
            l.unlock();
        }
        else
        {
            l.unlock();
        }

        if (stop_execution)
        {
            // Exit loop if appropriate
            break;
        }
        else if (!was_running_before)
        {
            // Otherwise notify continuation of thread
            auto r = std::unique_lock<std::mutex>(_mutex_run);
            _running = true;
            _sem_running.notify_all();
            _on_resume();
            was_running_before = true;
        }

        // If continuing execution, do the task
        _task();
    }
    _on_finalize();

    auto f = std::unique_lock<std::mutex>(_mutex_finalized);
    _finalized = true;
    _sem_finalized.notify_all();
}

} // namespace tools::thread