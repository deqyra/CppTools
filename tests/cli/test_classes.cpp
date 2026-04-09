#include "test_classes.hpp"

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/command.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/cli/input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>

namespace tools::cli
{

std::string_view test_command1::tooltip() const
{
    return "test_command1";
}

auto test_command1::run(test_env& env) -> code
{
    env.cli_streams().out << "test_command1 was run.";
    return code::success;
}

std::string_view test_command2::tooltip() const
{
    return "test_command2";
}

auto test_command2::run(test_env& env) -> code
{
    env.cli_streams().out << "test_command2 was run.";
    return code::success;
}

std::string_view test_exit_command::tooltip() const
{
    return "Exit";
}

auto test_exit_command::run(test_env& env) -> code
{
    env.cli_streams().out << "Successful exit.";
    return code::success;
}

test_menu make_basic_test_menu()
{
    return test_menu(
        {std::make_unique<test_command1>(), std::make_unique<test_command2>()},
        "Test menu", "Exit", std::make_unique<test_exit_command>()
    );
}

test_command_sequence make_basic_test_command_sequence()
{
    return test_command_sequence(
        {
            std::make_unique<test_menu_command>(make_basic_test_menu()),
            std::make_unique<test_command1>(),
            std::make_unique<test_command2>()
        },
        "Test sequence"
    );
}

auto test_shell_cmd::run(std::string_view command, test_env& env) -> code {
    env.shell_streams().out << "test_shell_cmd was run.\n";
    return code::success;
}

auto test_shell_cmd_throw::run(std::string_view command, test_env& env) -> code {
    struct ex : std::exception {
        const char* what() const noexcept {
            return "prout";
        }
    };

    throw ex{};
}

auto test_shell_exit_command::run(std::string_view command, test_env& env) -> code {
    env.shell_streams().out << "test_shell_exit_command was run.\n";
    return code::exit;
}

auto test_shell_help_command::run(std::string_view command, test_env& env) -> code {
    static constexpr std::string_view GlobalText =
R"(Available commands:
  - test_shell_cmd: Run test_shell_cmd
  - test_shell_cmd_throw: Run test_shell_cmd_throw
)";

    env.shell_streams().out << GlobalText;

    return code::success;
}

} // namespace tools::cli
