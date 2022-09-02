#include <catch2/catch2.hpp>

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shell_test_classes.hpp"

#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/utility/string_tools.hpp>

#define TAGS "[micro_shell]"

namespace tools
{

TEST_CASE("micro_shell")
{
    test_shell shell = test_shell();
    test_shell_command_ptr com1 = std::make_shared<test_shell_command1>();
    test_shell_command_ptr com2 = std::make_shared<test_shell_command2>();
    test_shell_command_ptr exit = std::make_shared<test_shell_exit_command>();

    SECTION("add_command")
    {
        REQUIRE_NOTHROW(shell.add_command(com1));
    }

    SECTION("has_command")
    {
        REQUIRE_NOTHROW(shell.add_command(com1));
        REQUIRE(shell.has_command("test_shell_command1"));
    }

    SECTION("get_command")
    {
        REQUIRE_NOTHROW(shell.add_command(com1));
        REQUIRE(shell.get_command("test_shell_command1") == com1);
    }

    SECTION("index_of")
    {
        REQUIRE_NOTHROW(shell.add_command(com1));
        REQUIRE(shell.index_of("test_shell_command1") == 0);
    }

    SECTION("set and get_exit_command")
    {
        REQUIRE_NOTHROW(shell.set_exit_command(exit));
        REQUIRE(shell.get_exit_command() == exit);
    }

    SECTION("remove_command")
    {
        REQUIRE_NOTHROW(shell.add_command(com1));
        REQUIRE_NOTHROW(shell.remove_command("test_shell_command1"));
        REQUIRE_FALSE(shell.has_command("test_shell_command1"));
    }

    SECTION("End to end")
    {
        REQUIRE_NOTHROW(shell.add_command(com1));
        REQUIRE_NOTHROW(shell.add_command(com2));
        REQUIRE_NOTHROW(shell.set_exit_command(exit));

        std::ifstream f = std::ifstream("resources/tests/tools/micro_shell/micro_shell_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        cli_streams s = cli_streams(f, ss, ss);
        test_shell_state state = test_shell_state();

        std::string expected = String::read_file_into_string("resources/tests/tools/micro_shell/micro_shell_output.txt");
        REQUIRE_NOTHROW(shell.run(state, s));
        REQUIRE(ss.str() == expected);
    }
}

} // namespace tools
