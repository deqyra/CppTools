#ifndef CPPTOOLS_OO_INTERFACES_ACTION_EVENT_RECEIVER
#define CPPTOOLS_OO_INTERFACES_ACTION_EVENT_RECEIVER

#include <memory>

namespace tools
{

/// @brief Interface for an object able to receive and process events related to
/// an action of a certain type.
///
/// @tparam T Type of the action to be handled by the implementing class.
template<typename T>
class action_event_receiver
{
public:
    using action_type = T;

    /// @brief Start the processing for an action.
    ///
    /// @param action Object describing the action to start processing.
    virtual void trigger_action(const T& action) = 0;

    /// @brief Stop the processing for an action.
    ///
    /// @param action Object describing the action to stop processing.
    virtual void stop_action(const T& action) = 0;
};

template<typename T>
using action_event_receiver_ptr = std::unique_ptr<action_event_receiver<T>>;

} // namespace tools::oo

#endif//CPPTOOLS_OO_INTERFACES_ACTION_EVENT_RECEIVER
