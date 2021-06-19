#ifndef CPPTOOLS__THREAD__INTERFACES__INTERRUPTIBLE_HPP
#define CPPTOOLS__THREAD__INTERFACES__INTERRUPTIBLE_HPP

namespace cpptools
{

class Interruptible
{
public:
    /// @brief Ask for the threaded function to stop processing and cleanly exit.
    virtual void finalize() = 0;

    /// @brief Pause the execution of the threaded function.
    virtual void pause() = 0;
    
    /// @brief Resume the execution of the threaded function.
    virtual void resume() = 0;
};


}

#endif//CPPTOOLS__THREAD__INTERFACES__INTERRUPTIBLE_HPP