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

TEST_CASE("command_sequence end-to-end", TAGS)
{
    std::ifstream f = std::ifstream("resources/tests/tools/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    cli_streams s = cli_streams(f, ss, ss);
    TestCLIState state = TestCLIState();

    TestCLIcommand_sequence command_sequence = make_basic_test_command_sequence();

    command_sequence.run(state, s);

    std::string expected = string::from_file("resources/tests/tools/cli/menu_output.txt");
    expected += "Testcli_command1 was run.\n"
                "Testcli_command2 was run.\n";

    std::string str = ss.str();

    REQUIRE(ss.str() == expected);

    f.close();
}

} // namespace tools
