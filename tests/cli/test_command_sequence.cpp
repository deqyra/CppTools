#include <catch2/catch_all.hpp>

#include "test_classes.hpp"

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/utility/string.hpp>

#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

#define TAGS "[cli][command]"

namespace tools::cli
{

TEST_CASE("command_sequence end-to-end", TAGS)
{
    auto a = std::filesystem::current_path();

    std::ifstream f = std::ifstream("resources/cli/menu_input.txt", std::ios::in);
    REQUIRE(f);
    std::stringstream ss;

    streams s = streams{f, ss, ss};
    test_state state = test_state();

    test_command_sequence<3> command_sequence = make_basic_test_command_sequence();

    command_sequence.run(state, s);

    std::string expected = from_file("resources/cli/menu_output.txt");
    expected += "test_command1 was run.\n"
                "test_command2 was run.\n";

    std::string str = ss.str();

    REQUIRE(ss.str() == expected);

    f.close();
}

} // namespace tools::cli