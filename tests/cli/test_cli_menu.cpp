#include <catch2/catch2.hpp>

#include "cli_test_classes.hpp"
#include <tools/cli/cli_menu.hpp>
#include <tools/cli/cli_streams.hpp>
#include <tools/string_tools.hpp>

#include <string>
#include <fstream>
#include <sstream>

#define TAGS "[cli][cli_command]"

namespace tools
{

TEST_CASE("CLIMenu end-to-end", TAGS)
{
    std::ifstream f = std::ifstream("resources/tests/tools/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    CLIStreams s = CLIStreams(f, ss, ss);
    TestCLIState state = TestCLIState();

    TestCLIMenu menu = makeBasicTestMenu();
    menu.show(state, s);

    std::string expected = String::readFileIntoString("resources/tests/tools/cli/menu_output.txt");

    REQUIRE(ss.str() == expected);

    f.close();
}

} // namespace tools
