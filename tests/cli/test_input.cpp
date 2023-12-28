#include <catch2/catch_all.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <cpptools/cli/input.hpp>

#define TAGS "[cli][input]"

namespace tools::cli
{

TEST_CASE("CLI input", TAGS)
{
    SECTION("Int input")
    {
        std::ifstream f = std::ifstream("resources/cli/int_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        streams s = streams{f, ss, ss};

        REQUIRE(input::prompt<int>("input1: ", s) == 30);
        REQUIRE(input::prompt<int>("input2: ", s) == -20);
        REQUIRE(input::prompt_min<int>("min: ", 40, s) == 40);
        REQUIRE(input::prompt_max<int>("max: ", 40, s) == 40);
        REQUIRE(input::prompt_bounded<int>("bounded: ", 15, 25, s) == 20);

        std::string expected =
            "input1: Please enter a integer.\n"
            "input1: input2: min: Please enter a value greater than 40.\n"
            "min: max: Please enter a value less than 40.\n"
            "max: bounded: Please enter a value between 15 and 25.\n"
            "bounded: Please enter a value between 15 and 25.\n"
            "bounded: ";

        REQUIRE(ss.str() == expected);

        f.close();
    }

    SECTION("Bool input")
    {
        std::ifstream f = std::ifstream("resources/cli/bool_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        streams s = streams{f, ss, ss};

        REQUIRE(input::prompt<bool>("inputY: ", s));
        REQUIRE(input::prompt<bool>("input_yes: ", s));
        REQUIRE(input::prompt<bool>("input_true: ", s));
        REQUIRE_FALSE(input::prompt<bool>("inputN: ", s));
        REQUIRE_FALSE(input::prompt<bool>("input_no: ", s));
        REQUIRE_FALSE(input::prompt<bool>("input_false: ", s));

        std::string expected =
            "inputY: Please enter a boolean (\"y\", \"yes\", \"true\", \"n\", \"no\", \"false\").\n"
            "inputY: input_yes: input_true: inputN: input_no: input_false: ";

        REQUIRE(ss.str() == expected);

        f.close();
    }
}

} // namespace tools::cli
