#include <catch2/catch2.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <cpptools/cli/cli_input.hpp>

#define TAGS "[cli][cli_input]"

namespace tools
{

TEST_CASE("CLI input", TAGS)
{
    SECTION("Int input")
    {
        std::ifstream f = std::ifstream("resources/tests/tools/cli/int_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        cli_streams s = cli_streams(f, ss, ss);

        REQUIRE(cli_input::ask_for_input<int>("input1: ", s) == 30);
        REQUIRE(cli_input::ask_for_input<int>("input2: ", s) == -20);
        REQUIRE(cli_input::ask_for_min_input<int>("min_input: ", 40, s) == 40);
        REQUIRE(cli_input::ask_for_max_input<int>("max_input: ", 40, s) == 40);
        REQUIRE(cli_input::ask_for_bounded_input<int>("bounded_input: ", 15, 25, s) == 20);

        std::string expected =  "input1: Invalid input, please enter a integer.\n"
                                "input1: input2: min_input: Invalid input, please enter a value that is at least 40.\n"
                                "min_input: max_input: Invalid input, please enter a value that is at most 40.\n"
                                "max_input: bounded_input: Invalid input, please enter a value between 15 and 25.\n"
                                "bounded_input: Invalid input, please enter a value between 15 and 25.\n"
                                "bounded_input: ";

        REQUIRE(ss.str() == expected);

        f.close();
    }

    SECTION("Bool input")
    {
        std::ifstream f = std::ifstream("resources/tests/tools/cli/bool_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        cli_streams s = cli_streams(f, ss, ss);

        REQUIRE(cli_input::ask_for_input<bool>("inputY: ", s));
        REQUIRE(cli_input::ask_for_input<bool>("input_yes: ", s));
        REQUIRE(cli_input::ask_for_input<bool>("input_true: ", s));
        REQUIRE_FALSE(cli_input::ask_for_input<bool>("inputN: ", s));
        REQUIRE_FALSE(cli_input::ask_for_input<bool>("input_no: ", s));
        REQUIRE_FALSE(cli_input::ask_for_input<bool>("input_false: ", s));

        std::string expected =  "inputY: Invalid input, please enter a boolean (\"y\", \"yes\", \"true\", \"n\", \"no\", \"false\").\n"
                                "inputY: input_yes: input_true: inputN: input_no: input_false: ";

        REQUIRE(ss.str() == expected);

        f.close();
    }
}

} // namespace tools
