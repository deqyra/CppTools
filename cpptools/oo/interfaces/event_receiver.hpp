#ifndef CPPTOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP
#define CPPTOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP

#include <memory>

namespace tools
{

/// @brief Interface for a class able to receive events and queue them.
///
/// @tparam E Type of events to be queued. 
template<typename E>
class event_receiver
{
public:
    /// @brief Register a critical event to the window.
    ///
    /// @param event Literal describing the event to queue.
    virtual void queue_event(const E event) = 0;
};

template<typename E>
using event_receiver_ptr = std::unique_ptr<event_receiver<E>>;

} // namespace tools


#endif//CPPTOOLS__OO__INTERFACES__EVENT_RECEIVER_HPP