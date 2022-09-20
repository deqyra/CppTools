#include "test_classes.hpp"

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/command.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/cli/input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>

namespace tools::cli
{

std::string test_command1::tooltip() const
{
    return "test_command1";
}

auto test_command1::run(test_state& state, streams& streams) -> code
{
    streams.out << "test_command1 was run.\n";
    return code::success;
}

std::string test_command2::tooltip() const
{
    return "test_command2";
}

auto test_command2::run(test_state& state, streams& streams) -> code
{
    streams.out << "test_command2 was run.\n";
    return code::success;
}

std::string test_exit_command::tooltip() const
{
    return "Exit";
}

auto test_exit_command::run(test_state& state, streams& streams) -> code
{
    streams.out << "Successful exit.\n";
    return code::success;
}

test_menu<2> make_basic_test_menu()
{
    return test_menu<2>(
        {std::make_unique<test_command1>(), std::make_unique<test_command2>()},
        "Test menu", "Exit", std::make_unique<test_exit_command>()
    );
}

test_command_sequence<3> make_basic_test_command_sequence()
{
    return test_command_sequence<3>(
        {
            std::make_unique<test_menu_command<2>>(make_basic_test_menu()),
            std::make_unique<test_command1>(),
            std::make_unique<test_command2>()
        },
        "Test sequence"
    );
}

std::string test_shell_command1::name() const
{
    return "test_shell_command1";
}

std::string test_shell_command1::description() const
{
    return "Run test_shell_command1";
}

std::string test_shell_command1::help() const
{
    return "test_shell_command1: no arguments expected.";
}

auto test_shell_command1::process_input(std::string_view command, test_state& state, streams& streams) -> code
{
    streams.out << "test_shell_command1 was run.\n";
    return code::success;
}

std::string test_shell_command2::name() const
{
    return "test_shell_command2";
}

std::string test_shell_command2::description() const
{
    return "Run test_shell_command2";
}

std::string test_shell_command2::help() const
{
    return "test_shell_command2: no arguments expected.";
}

auto test_shell_command2::process_input(std::string_view command, test_state& state, streams& streams) -> code
{
    streams.out << "test_shell_command2 was run.\n";
    return code::success;
}

std::string test_shell_exit_command::name() const
{
    return "test_shell_exit_command";
}

std::string test_shell_exit_command::description() const
{
    return "Run test_shell_exit_command";
}

std::string test_shell_exit_command::help() const
{
    return "test_shell_exit_command: no arguments expected.";
}

auto test_shell_exit_command::process_input(std::string_view command, test_state& state, streams& streams) -> code
{
    streams.out << "test_shell_exit_command was run.\n";
    return code::exit;
}


} // namespace tools::cli
