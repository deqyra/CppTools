#ifndef CPPTOOLS_CLI_COMMAND_HPP
#define CPPTOOLS_CLI_COMMAND_HPP

#include <string>
#include <string_view>

#include <cpptools/api.hpp>

#include "streams.hpp"
#include "input.hpp"

namespace tools::cli {

enum class command_code {
    success = 0,
    failure = -1
};

CPPTOOLS_API std::string to_string(command_code c);

template<typename Context>
class command {
public:
    using code = command_code;

    virtual ~command() = default;

    // Display name of the command.
    virtual std::string_view tooltip() const = 0;

    // Run the command.
    virtual code run(Context& state, streams& streams = input::default_streams) = 0;
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_COMMAND_HPP