#ifndef CPPTOOLS__CLI__COMMAND_SEQUENCE_HPP
#define CPPTOOLS__CLI__COMMAND_SEQUENCE_HPP

#include <vector>
#include <string>
#include <memory>

#include "cli_command.hpp"
#include "cli_input.hpp"

namespace tools
{

// Extended command allowing to run several commands in a row.
template <typename state_t>
class command_sequence : public cli_command<state_t>
{
public:
    using command_ptr = std::unique_ptr<cli_command<state_t>>;
    using code = typename cli_command<state_t>::code;

private:
    // Ordered commands to run.
    std::vector<command_ptr> _command_list;

    // Display name of the command sequence.
    std::string _tooltip;

public: 
    command_sequence(std::vector<command_ptr> command_list, std::string tooltip = "") :
        cli_command<state_t>(),
        _command_list(command_list),
        _tooltip(tooltip)
    {

    }

    virtual ~command_sequence() = default;

    virtual std::string get_tooltip()
    {
        if (!_tooltip.empty())
        {
            return _tooltip;
        }
        return _command_list[0]->get_tooltip();
    }

    // Run the command sequence.
    virtual code run(state_t& state, cli_streams& streams = cli_input::default_streams)
    {
        // Run all commands in order.
        for (auto it = _command_list.begin(); it != _command_list.end(); it++)
        {
            int status;
            try
            {
                status = (*it)->run(state, streams);
            }
            catch (const std::exception& e)
            {
                // Informative error logging.
                streams.err << "Exception thrown:\n";
                streams.err << e.what() << '\n';
                streams.out << "Command sequence \"" + _tooltip + "\" aborted." << std::endl;
                return code::failure;
            }

            // If the command failed (but didn't throw)...
            if (status != code::success)
            {
                // Log error and return prematurely.
                streams.out << "Command \"" + (*it)->get_tooltip() + "\" returned with value " + std::to_string(status) + "\n";
                streams.out << "Command sequence \"" + _tooltip + "\" aborted." << std::endl;
                return status;
            }
        }

        return code::success;
    }
};


} // namespace tools

#endif//CPPTOOLS__CLI__COMMAND_SEQUENCE_HPP