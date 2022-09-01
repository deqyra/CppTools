#ifndef TOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP
#define TOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP

#include <memory>

namespace tools
{

/// @brief Interface for a class able to receive events and queue them.
///
/// @tparam E Type of events to be queued. 
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

} // namespace tools


#endif//TOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP