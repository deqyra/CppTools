#include <catch2/catch2.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <cpptools/cli/cli_input.hpp>

#define TAGS "[cli][cli_input]"

namespace CppTools
{

TEST_CASE("CLI input", TAGS)
{
    SECTION("Int input")
    {
        std::ifstream f = std::ifstream("resources/tests/tools/cli/int_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        CLIStreams s = CLIStreams(f, ss, ss);

        REQUIRE(CLIInput::askForInput<int>("input1: ", s) == 30);
        REQUIRE(CLIInput::askForInput<int>("input2: ", s) == -20);
        REQUIRE(CLIInput::askForMinInput<int>("minInput: ", 40, s) == 40);
        REQUIRE(CLIInput::askForMaxInput<int>("maxInput: ", 40, s) == 40);
        REQUIRE(CLIInput::askForBoundedInput<int>("boundedInput: ", 15, 25, s) == 20);

        std::string expected =  "input1: Invalid input, please enter a integer.\n"
                                "input1: input2: minInput: Invalid input, please enter a value that is at least 40.\n"
                                "minInput: maxInput: Invalid input, please enter a value that is at most 40.\n"
                                "maxInput: boundedInput: Invalid input, please enter a value between 15 and 25.\n"
                                "boundedInput: Invalid input, please enter a value between 15 and 25.\n"
                                "boundedInput: ";

        REQUIRE(ss.str() == expected);

        f.close();
    }

    SECTION("Bool input")
    {
        std::ifstream f = std::ifstream("resources/tests/tools/cli/bool_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        CLIStreams s = CLIStreams(f, ss, ss);

        REQUIRE(CLIInput::askForInput<bool>("inputY: ", s));
        REQUIRE(CLIInput::askForInput<bool>("inputYes: ", s));
        REQUIRE(CLIInput::askForInput<bool>("inputTrue: ", s));
        REQUIRE_FALSE(CLIInput::askForInput<bool>("inputN: ", s));
        REQUIRE_FALSE(CLIInput::askForInput<bool>("inputNo: ", s));
        REQUIRE_FALSE(CLIInput::askForInput<bool>("inputFalse: ", s));

        std::string expected =  "inputY: Invalid input, please enter a boolean (\"y\", \"yes\", \"true\", \"n\", \"no\", \"false\").\n"
                                "inputY: inputYes: inputTrue: inputN: inputNo: inputFalse: ";

        REQUIRE(ss.str() == expected);

        f.close();
    }
}

}//namespace CppTools
