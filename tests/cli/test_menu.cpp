#include <catch2/catch_all.hpp>

#include "test_classes.hpp"
#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/utility/string.hpp>

#define TAGS "[cli][command]"

namespace tools::cli
{

TEST_CASE("menu end-to-end", TAGS)
{
    auto env = test_env("resources/cli/menu_input.txt");

    test_menu menu = make_basic_test_menu();
    menu.show(env);

    auto expected = from_file("resources/cli/menu_output.txt");

    REQUIRE(env.out.str() == expected);
}

} // namespace tools::cli
