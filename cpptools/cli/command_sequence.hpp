#ifndef CPPTOOLS__CLI__COMMAND_SEQUENCE_HPP
#define CPPTOOLS__CLI__COMMAND_SEQUENCE_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "command.hpp"
#include "input.hpp"

namespace tools::cli
{

// Extended command allowing to run several commands in a row.
template <typename context_t, std::size_t N>
class command_sequence : public cli::command<context_t>
{
public:
    using command_ptr = std::unique_ptr<cli::command<context_t>>;
    using code = typename cli::command<context_t>::code;

private:
    // Ordered commands to run.
    std::array<command_ptr, N> _commands;

    // Display name of the command sequence.
    std::string _tooltip;

public:
    command_sequence(std::array<command_ptr, N>&& commands, std::string tooltip = "") :
        cli::command<context_t>(),
        _commands(std::move(commands)),
        _tooltip(std::move(tooltip))
    {

    }

    virtual ~command_sequence() = default;

    virtual std::string tooltip() const override
    {
        return _tooltip;
    }

    // Run the command sequence.
    virtual code run(context_t& state, cli::streams& streams = cli::input::default_streams) override
    {
        // Run all commands in order.
        for (auto& command : _commands)
        {
            code status;
            try
            {
                status = command->run(state, streams);
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
                streams.out << "Command \"" + command->tooltip() + "\" returned with value " + to_string(status) + "\n";
                streams.out << "Command sequence \"" + _tooltip + "\" aborted." << std::endl;
                return status;
            }
        }

        return code::success;
    }
};

} // namespace tools::cli

#endif//CPPTOOLS__CLI__COMMAND_SEQUENCE_HPP