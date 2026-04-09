#ifndef CPPTOOLS_CLI_COMMAND_SEQUENCE_HPP
#define CPPTOOLS_CLI_COMMAND_SEQUENCE_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <cpptools/exception/parameter_exception.hpp>

#include "command.hpp"

namespace tools::cli {

// Extended command allowing to run several commands in a row.
template <cli_environment env_t>
class command_sequence : public command<env_t> {
public:
    using command_ptr = std::unique_ptr<command<env_t>>;
    using code = typename command<env_t>::code;

private:
    // Ordered commands to run.
    std::vector<command_ptr> _commands;

    // Display name of the command sequence.
    std::string _tooltip;

public:
    template<std::size_t N>
    command_sequence(command_ptr(&&commands)[N], std::string tooltip) :
        command<env_t>(),
        _commands(),
        _tooltip(std::move(tooltip))
    {
        std::size_t i = 0;
        _commands.reserve(N);
        for (auto& c : commands) {
            if (c == nullptr) {
                CPPTOOLS_THROW(exception::parameter::null_parameter_error, "commands[" + std::to_string(i) + "]");
            }
            ++i;
            _commands.push_back(std::move(c));
        }
    }

    virtual ~command_sequence() = default;

    virtual std::string_view tooltip() const override {
        return _tooltip;
    }

    virtual code run(env_t& env) override {
        auto& streams = env.cli_streams();

        for (auto& command : _commands) {
            code status;
            try {
                status = command->run(env);
            } catch (const std::exception& e) {
                streams.err << std::format("Exception thrown by command \"{}\":\n{}", command->tooltip(), e.what()) << std::endl;
                streams.out << std::format("Command sequence \"{}\" aborted.", _tooltip) << std::endl;
                return code::failure;
            }

            if (status != code::success) {
                streams.err << std::format("Command \"{}\" returned status code {}.", command->tooltip(), std::to_underlying(status)) << std::endl;
                streams.out << std::format("Command sequence \"{}\" aborted.", _tooltip) << std::endl;
                return status;
            }
        }

        return code::success;
    }
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_COMMAND_SEQUENCE_HPP