#ifndef CPPTOOLS__CLI__CLI_COMMAND_HPP
#define CPPTOOLS__CLI__CLI_COMMAND_HPP

#include <string>
#include <memory>

#include "cli_streams.hpp"
#include "cli_input.hpp"

namespace tools
{

template<typename state_t>
class cli_command
{
public:
    enum class code : int
    {
        success = 0,
        failure = -1
    };

    virtual ~cli_command() = default;

    // Display name of the command.
    virtual std::string get_tooltip() = 0;

    // Run the command.
    virtual int run(state_t& state, cli_streams& streams = cli_input::default_streams) = 0;
};

} // namespace tools

#endif//CPPTOOLS__CLI__CLI_COMMAND_HPP