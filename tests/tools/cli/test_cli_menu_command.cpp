#include <catch2/catch2.hpp>

#include "cli_test_classes.hpp"
#include <cpptools/cli/cli_menu.hpp>
#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/string_tools.hpp>

#include <string>
#include <fstream>
#include <sstream>

#define TAGS "[cli][cli_command]"

namespace CppTools
{

TEST_CASE("MenuCommand end-to-end", TAGS)
{
    std::ifstream f = std::ifstream("resources/tests/tools/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    CLIStreams s = CLIStreams(f, ss, ss);
    TestCLIState state = TestCLIState();

    TestCLIMenu menu = makeBasicTestMenu();
    TestCLIMenuCommand menuCommand = TestCLIMenuCommand(menu);
    menuCommand.run(state, s);

    std::string expected = String::readFileIntoString("resources/tests/tools/cli/menu_output.txt");

    REQUIRE(ss.str() == expected);

    f.close();
}

}//namespace CppTools
