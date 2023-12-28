#include <catch2/catch_all.hpp>

#include "test_classes.hpp"

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/utility/string.hpp>

#include <string>
#include <fstream>
#include <sstream>

#define TAGS "[cli][command]"

namespace tools::cli
{

TEST_CASE("menu_command end-to-end", TAGS)
{
    std::ifstream f = std::ifstream("resources/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    streams s{f, ss, ss};
    test_state state = test_state();

    test_menu_command<2> menu_command{make_basic_test_menu()};
    menu_command.run(state, s);

    std::string expected = string::from_file("resources/cli/menu_output.txt");

    REQUIRE(ss.str() == expected);

    f.close();
}

} // namespace tools::cli
