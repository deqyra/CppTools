#include <catch2/catch2.hpp>

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "test_classes.hpp"

#include <cpptools/cli/streams.hpp>
#include <cpptools/utility/string.hpp>

#define TAGS "[cli][shell]"

namespace tools::cli
{

TEST_CASE("shell")
{
    test_state state;
    test_shell_command1 com1{};
    test_shell_command2 com2{};
    test_shell_exit_command exit{};
    test_shell shell{
        std::array<test_shell_command_ptr, 1>{std::make_unique<test_shell_command2>(com2)},
        std::make_unique<test_shell_exit_command>(exit)
    };

    SECTION("add_command")
    {
        REQUIRE_NOTHROW(shell.add_command(std::make_unique<test_shell_command1>(com1)));
    }

    SECTION("has_command")
    {
        REQUIRE_FALSE(shell.has_command("test_shell_command1"));
        REQUIRE_NOTHROW(shell.add_command(std::make_unique<test_shell_command1>(com1)));
        REQUIRE(shell.has_command("test_shell_command1"));
    }

    SECTION("get_command")
    {
        REQUIRE_NOTHROW(shell.add_command(std::make_unique<test_shell_command1>(com1)));
        REQUIRE(shell.get_command("test_shell_command1").name() == com1.name());
    }

    SECTION("set and get_exit_command")
    {
        REQUIRE_NOTHROW(shell.set_exit_command(std::make_unique<test_shell_exit_command>(exit)));
        REQUIRE(shell.get_exit_command().name() == exit.name());
    }

    SECTION("remove_command")
    {
        REQUIRE_NOTHROW(shell.add_command(std::make_unique<test_shell_command1>(com1)));
        REQUIRE_NOTHROW(shell.remove_command("test_shell_command1"));
        REQUIRE_FALSE(shell.has_command("test_shell_command1"));
    }

    SECTION("End to end")
    {
        REQUIRE_NOTHROW(shell.add_command(std::make_unique<test_shell_command1>(com1)));

        std::ifstream f = std::ifstream("resources/cli/shell_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        streams s = streams{f, ss, ss};
        test_state state = test_state();

        std::string expected = string::from_file("resources/cli/shell_output.txt");
        REQUIRE_NOTHROW(shell.run(state, s));
        REQUIRE(ss.str() == expected);
    }
}

} // namespace tools::cli
