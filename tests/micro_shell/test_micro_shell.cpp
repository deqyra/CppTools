#include <catch2/catch2.hpp>

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shell_test_classes.hpp"

#include <tools/cli/cli_streams.hpp>
#include <tools/string_tools.hpp>

#define TAGS "[micro_shell]"

namespace tools
{

TEST_CASE("MicroShell")
{
    TestShell shell = TestShell();
    TestShellCommandPtr com1 = std::make_shared<TestShellCommand1>();
    TestShellCommandPtr com2 = std::make_shared<TestShellCommand2>();
    TestShellCommandPtr exit = std::make_shared<TestShellExitCommand>();
    
    SECTION("addCommand")
    {
        REQUIRE_NOTHROW(shell.addCommand(com1));
    }

    SECTION("hasCommand")
    {
        REQUIRE_NOTHROW(shell.addCommand(com1));
        REQUIRE(shell.hasCommand("TestShellCommand1"));
    }

    SECTION("getCommand")
    {
        REQUIRE_NOTHROW(shell.addCommand(com1));
        REQUIRE(shell.getCommand("TestShellCommand1") == com1);
    }

    SECTION("indexOf")
    {
        REQUIRE_NOTHROW(shell.addCommand(com1));
        REQUIRE(shell.indexOf("TestShellCommand1") == 0);
    }

    SECTION("set and getExitCommand")
    {
        REQUIRE_NOTHROW(shell.setExitCommand(exit));
        REQUIRE(shell.getExitCommand() == exit);
    }

    SECTION("removeCommand")
    {
        REQUIRE_NOTHROW(shell.addCommand(com1));
        REQUIRE_NOTHROW(shell.removeCommand("TestShellCommand1"));
        REQUIRE_FALSE(shell.hasCommand("TestShellCommand1"));
    }

    SECTION("End to end")
    {
        REQUIRE_NOTHROW(shell.addCommand(com1));
        REQUIRE_NOTHROW(shell.addCommand(com2));
        REQUIRE_NOTHROW(shell.setExitCommand(exit));

        std::ifstream f = std::ifstream("resources/tests/tools/micro_shell/micro_shell_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        CLIStreams s = CLIStreams(f, ss, ss);
        TestShellState state = TestShellState();

        std::string expected = String::readFileIntoString("resources/tests/tools/micro_shell/micro_shell_output.txt");
        REQUIRE_NOTHROW(shell.run(state, s));
        REQUIRE(ss.str() == expected);
    }
}

} // namespace tools
