#include <catch2/catch2.hpp>

#include "cli_test_classes.hpp"
#include <cpptools/cli/cli_menu.hpp>
#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/utility/string_tools.hpp>

#include <string>
#include <fstream>
#include <sstream>

#define TAGS "[cli][cli_command]"

namespace tools
{

TEST_CASE("cli_menu end-to-end", TAGS)
{
    std::ifstream f = std::ifstream("resources/tests/tools/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    cli_streams s = cli_streams(f, ss, ss);
    TestCLIState state = TestCLIState();

    Testcli_menu menu = make_basic_test_menu();
    menu.show(state, s);

    std::string expected = String::read_file_into_string("resources/tests/tools/cli/menu_output.txt");

    REQUIRE(ss.str() == expected);

    f.close();
}

} // namespace tools
