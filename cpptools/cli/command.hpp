#ifndef CPPTOOLS_CLI_COMMAND_HPP
#define CPPTOOLS_CLI_COMMAND_HPP

#include <string_view>

#include <cpptools/api.hpp>

#include "streams.hpp"

namespace tools::cli {

enum class command_code {
    success = 0,
    failure = -1
};

CPPTOOLS_API inline std::string_view to_string(command_code c) {
    switch (c) {
        case command_code::success:
            return "success";
        case command_code::failure:
            return "failure";
        default:
            return "???";
    }
}

template<typename Context>
class command {
public:
    using code = command_code;

    virtual ~command() = default;

    // Display name of the command.
    virtual std::string_view tooltip() const = 0;

    // Run the command.
    virtual code run(Context& state, streams& streams = default_streams) = 0;
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_COMMAND_HPP