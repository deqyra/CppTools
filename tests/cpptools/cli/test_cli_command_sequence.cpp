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

TEST_CASE("CommandSequence end-to-end", TAGS)
{
    std::ifstream f = std::ifstream("resources/tests/tools/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    CLIStreams s = CLIStreams(f, ss, ss);
    TestCLIState state = TestCLIState();

    TestCLICommandSequence commandSequence = makeBasicTestCommandSequence();

    commandSequence.run(state, s);

    std::string expected = string::from_file("resources/tests/tools/cli/menu_output.txt");
    expected += "TestCLICommand1 was run.\n"
                "TestCLICommand2 was run.\n";

    std::string str = ss.str();

    REQUIRE(ss.str() == expected);

    f.close();
}

} // namespace tools
