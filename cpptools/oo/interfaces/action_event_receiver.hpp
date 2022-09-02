#ifndef CPPTOOLS__OO__INTERFACES__ACTION_EVENT_RECEIVER
#define CPPTOOLS__OO__INTERFACES__ACTION_EVENT_RECEIVER

#include <memory>

#include <cpptools/utility/type_utils.hpp>

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
    /// @param args Arguments of the action to send along with the stop signal.
    virtual void trigger_action(const T& action) = 0;

    /// @brief Stop the processing for an action.
    ///
    /// @param action Object describing the action to stop processing.
    /// @param args Arguments of the action to send along with the stop signal.
    virtual void stop_action(const T& action) = 0;
};

template<typename T>
using action_event_receiver_ptr = std::unique_ptr<action_event_receiver<T>>;

} // namespace tools

#endif//CPPTOOLS__OO__INTERFACES__ACTION_EVENT_RECEIVER
