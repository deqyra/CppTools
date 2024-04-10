#ifndef CPPTOOLS_THREAD_INTERFACES_INTERRUPTIBLE_HPP
#define CPPTOOLS_THREAD_INTERFACES_INTERRUPTIBLE_HPP

#include <memory>

namespace tools::thread
{

/// @brief Interface for a class implementing an interruptible process.
class interruptible
{
public:
    /// @brief Ask for the threaded process to stop processing and cleanly exit.
    virtual void finalize() = 0;

    /// @brief Tells whether or not the threaded process is finalized.
    virtual bool finalized() = 0;

    /// @brief Hangs the calling thread until the threaded process is finalized.
    ///
    /// @param pause_now Whether or not to request the threaded process to finalize
    /// upon calling this function. Providing false may prove useful when the
    /// threaded process must be requested to finalize by another thread than the
    /// one calling this function, or if the threaded process has already been 
    /// requested to finalize.
    virtual void wait_until_finalized(bool pause_now = true) = 0;

    /// @brief Pause the execution of the threaded process.
    virtual void pause() = 0;

    /// @brief Tells whether or not the threaded process is paused.
    virtual bool paused() = 0;

    /// @brief Hangs the calling thread until the threaded process is paused.
    ///
    /// @param pause_now Whether or not to request the threaded process to pause
    /// upon calling this function. Providing false may prove useful when the
    /// threaded process must be requested to pause by another thread than the
    /// one calling this function, or if the threaded process has already been 
    /// requested to pause.
    virtual void wait_until_paused(bool pause_now = true) = 0;

    /// @brief Resume the execution of the threaded function.
    virtual void run() = 0;

    /// @brief Tells whether or not the threaded process is running.
    virtual bool running() = 0;

    /// @brief Hangs the calling thread until the threaded process is running.
    ///
    /// @param pause_now Whether or not to request the threaded process to pause
    /// upon calling this function. Providing false may prove useful when the
    /// threaded process must be requested to resume by another thread than the
    /// one calling this function, or if the threaded process has already been 
    /// requested to resume.
    virtual void wait_until_running(bool pause_now = true) = 0;
};

using interruptible_ptr = std::unique_ptr<interruptible>;

} // namespace tools::thread

#endif//CPPTOOLS_THREAD_INTERFACES_INTERRUPTIBLE_HPP