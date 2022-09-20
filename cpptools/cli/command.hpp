#ifndef CPPTOOLS__CLI__COMMAND_HPP
#define CPPTOOLS__CLI__COMMAND_HPP

#include <string>
#include <memory>

#include "streams.hpp"
#include "input.hpp"

namespace tools::cli
{

enum class cli_command_code
{
    success = 0,
    failure = -1
};

std::string to_string(cli_command_code c);

template<typename context_t>
class command
{
public:
    using code = cli_command_code;

    virtual ~command() = default;

    // Display name of the command.
    virtual std::string tooltip() const = 0;

    // Run the command.
    virtual code run(context_t& state, cli::streams& streams = cli::input::default_streams) = 0;
};

} // namespace tools::cli

#endif//CPPTOOLS__CLI__COMMAND_HPP