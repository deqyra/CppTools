#ifndef CPPTOOLS_CLI_COMMAND_SEQUENCE_HPP
#define CPPTOOLS_CLI_COMMAND_SEQUENCE_HPP

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "command.hpp"
#include "input.hpp"

namespace tools::cli {

// Extended command allowing to run several commands in a row.
template <typename Context, std::size_t N>
class command_sequence : public command<Context> {
public:
    using command_ptr = std::unique_ptr<command<Context>>;
    using code = typename command<Context>::code;

private:
    // Ordered commands to run.
    std::array<command_ptr, N> _commands;

    // Display name of the command sequence.
    std::string _tooltip;

public:
    command_sequence(std::array<command_ptr, N>&& commands, std::string tooltip = "") :
        command<Context>(),
        _commands(std::move(commands)),
        _tooltip(std::move(tooltip))
    {

    }

    virtual ~command_sequence() = default;

    virtual std::string_view tooltip() const override {
        return _tooltip;
    }

    // Run the command sequence.
    virtual code run(Context& state, streams& streams = input::default_streams) override {
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
                streams.out << "Command sequence \"" << _tooltip << "\" aborted." << std::endl;
                return code::failure;
            }

            // If the command failed (but didn't throw)...
            if (status != code::success)
            {
                // Log error and return prematurely.
                streams.out << "Command \"" << command->tooltip() << "\" returned with value " << to_string(status) << "\n";
                streams.out << "Command sequence \"" << _tooltip << "\" aborted." << std::endl;
                return status;
            }
        }

        return code::success;
    }
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_COMMAND_SEQUENCE_HPP