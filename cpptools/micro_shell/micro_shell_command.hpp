#ifndef CPPTOOLS__MICRO_SHELL__MICRO_SHELL_COMMAND_HPP
#define CPPTOOLS__MICRO_SHELL__MICRO_SHELL_COMMAND_HPP

#include <string>

#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/cli/cli_input.hpp>

namespace tools
{

template<typename state_t>
class micro_shell_command
{
public: 
    struct keywords
    {
        static constexpr char help[] = "help";
        static constexpr char exit[] = "exit";
    };

    micro_shell_command()
    {

    }

    virtual ~micro_shell_command() = default;

    virtual std::string name() = 0;
    virtual std::string description() = 0;
    virtual std::string help() = 0;
    virtual int process_input(const std::string& command, state_t& state, cli_streams& streams = cli_input::default_streams) = 0;
};

} // namespace tools

#endif//CPPTOOLS__MICRO_SHELL__MICRO_SHELL_COMMAND_HPP