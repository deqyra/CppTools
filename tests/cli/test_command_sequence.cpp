#include <catch2/catch_all.hpp>

#include "test_classes.hpp"

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/utility/string.hpp>

#define TAGS "[cli][command]"

namespace tools::cli
{

TEST_CASE("command_sequence end-to-end", TAGS)
{
    test_env env = test_env("resources/cli/menu_input.txt");

    test_command_sequence command_sequence = make_basic_test_command_sequence();

    command_sequence.run(env);

    auto expected = from_file("resources/cli/menu_output.txt");
    expected += "test_command1 was run."
                "test_command2 was run.";

    REQUIRE(env.out.str() == expected);
}

} // namespace tools::cli