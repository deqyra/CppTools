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

TEST_CASE("menu end-to-end", TAGS)
{
    std::ifstream f = std::ifstream("resources/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    streams s = streams{f, ss, ss};
    test_state state = test_state();

    test_menu<2> menu = make_basic_test_menu();
    menu.show(state, s);

    std::string expected = string::from_file("resources/cli/menu_output.txt");

    REQUIRE(ss.str() == expected);

    f.close();
}

} // namespace tools::cli
