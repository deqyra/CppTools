#ifndef CPPTOOLS__THREAD__INTERFACES__INTERRUPTIBLE_HPP
#define CPPTOOLS__THREAD__INTERFACES__INTERRUPTIBLE_HPP

#include <memory>

namespace cpptools
{

/// @brief Interface for a class implementing an interruptible process.
class Interruptible
{
public:
    /// @brief Ask for the threaded process to stop processing and cleanly exit.
    virtual void finalize() = 0;

    /// @brief Tells whether or not the threaded process is finalized.
    virtual bool finalized() = 0;

    /// @brief Hangs the calling thread until the threaded process is finalized.
    ///
    /// @param pauseNow Whether or not to request the threaded process to finalize
    /// upon calling this function. Providing false may prove useful when the
    /// threaded process must be requested to finalize by another thread than the
    /// one calling this function, or if the threaded process has already been 
    /// requested to finalize.
    virtual void waitUntilFinalized(bool pauseNow = true) = 0;

    /// @brief Pause the execution of the threaded process.
    virtual void pause() = 0;

    /// @brief Tells whether or not the threaded process is paused.
    virtual bool paused() = 0;

    /// @brief Hangs the calling thread until the threaded process is paused.
    ///
    /// @param pauseNow Whether or not to request the threaded process to pause
    /// upon calling this function. Providing false may prove useful when the
    /// threaded process must be requested to pause by another thread than the
    /// one calling this function, or if the threaded process has already been 
    /// requested to pause.
    virtual void waitUntilPaused(bool pauseNow = true) = 0;

    /// @brief Resume the execution of the threaded function.
    virtual void run() = 0;

    /// @brief Tells whether or not the threaded process is running.
    virtual bool running() = 0;

    /// @brief Hangs the calling thread until the threaded process is running.
    ///
    /// @param pauseNow Whether or not to request the threaded process to pause
    /// upon calling this function. Providing false may prove useful when the
    /// threaded process must be requested to resume by another thread than the
    /// one calling this function, or if the threaded process has already been 
    /// requested to resume.
    virtual void waitUntilRunning(bool pauseNow = true) = 0;
};

using InterruptiblePtr = std::shared_ptr<Interruptible>;

}

#endif//CPPTOOLS__THREAD__INTERFACES__INTERRUPTIBLE_HPP