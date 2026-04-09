#ifndef CPPTOOLS_CLI_COMMAND_HPP
#define CPPTOOLS_CLI_COMMAND_HPP

#include <string_view>

#include <cpptools/api.hpp>

#include "streams.hpp"

namespace tools::cli {

enum class menu_option_code {
    success = 0,
    failure = 1
};

template<typename E>
concept cli_environment = requires(E& env) {
    { env.cli_streams() } -> std::same_as<streams&>;
};

class default_cli_environment {
public:
    virtual streams& cli_streams() {
        return default_streams;
    }
};
static_assert(cli_environment<default_cli_environment>);

template<cli_environment env_t>
class command {
public:
    using code = menu_option_code;

    virtual ~command() = default;

    // Display name of the command.
    virtual std::string_view tooltip() const = 0;

    // Run the command.
    virtual code run(env_t& env) = 0;
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_COMMAND_HPP