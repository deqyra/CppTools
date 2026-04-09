#include <catch2/catch_all.hpp>

#include "test_classes.hpp"

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/utility/string.hpp>

#define TAGS "[cli][command]"

namespace tools::cli
{

TEST_CASE("menu_command end-to-end", TAGS)
{
    test_env env = test_env("resources/cli/menu_input.txt");

    test_menu_command menu_command{make_basic_test_menu()};
    menu_command.run(env);

    auto expected = from_file("resources/cli/menu_output.txt");

    REQUIRE(env.out.str() == expected);
}

} // namespace tools::cli
