#ifndef CPPTOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP
#define CPPTOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP

#include <memory>

namespace cpptools
{

/// @brief Interface for a class able to receive critical events which were
/// queued to a window. Such a class is to be used in context with windows.
///
/// @tparam E Type of the critical events to be received. 
template<typename E>
class EventReceiver
{
public:
    /// @brief Register a critical event to the window.
    ///
    /// @param event Literal describing the event to queue.
    virtual void queueEvent(const E event) = 0;
};

template<typename E>
using EventReceiverPtr = std::unique_ptr<EventReceiver<E>>;

} // namespace cpptools


#endif//CPPTOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP