#include <catch2/catch_all.hpp>

#include "test_classes.hpp"

#include <cpptools/cli/streams.hpp>
#include <cpptools/utility/string.hpp>

#define TAGS "[cli][shell]"

namespace tools::cli
{

TEST_CASE("shell")
{
    test_shell_cmd cmd;
    test_shell_cmd_throw cmd_throw;
    test_shell_help_command help;
    test_shell_exit_command exit;

    test_shell shell;
    shell.set_command("test_shell_cmd_throw", cmd_throw);
    shell.set_command("help", help);
    shell.set_command("exit", exit);

    SECTION("set_command")
    {
        REQUIRE_NOTHROW(shell.set_command("test_shell_cmd", cmd));
    }

    SECTION("remove_command")
    {
        REQUIRE_NOTHROW(shell.remove_command("test_shell_cmd_throw"));
    }

    SECTION("add_default_exit_command")
    {
        REQUIRE_NOTHROW(shell.add_default_exit_command());
        REQUIRE_NOTHROW(shell.add_default_exit_command("test"));
    }

    SECTION("End to end")
    {
        REQUIRE_NOTHROW(shell.set_command("test_shell_cmd", cmd));

        auto env = test_env("resources/cli/shell_input.txt");

        auto expected = from_file("resources/cli/shell_output.txt");
        auto expected_err = from_file("resources/cli/shell_error_output.txt");

        REQUIRE_NOTHROW(shell.run(env));
        REQUIRE(env.out.str() == expected);
        REQUIRE(env.err.str() == expected_err);
    }
}

} // namespace tools::cli
