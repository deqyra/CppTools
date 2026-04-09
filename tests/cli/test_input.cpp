#include <catch2/catch_all.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <cpptools/cli/input.hpp>

#define TAGS "[cli][input]"

namespace tools::cli::test
{

TEST_CASE("CLI input", TAGS)
{
    SECTION("Int input")
    {
        std::ifstream f = std::ifstream("resources/cli/int_input.txt", std::ios::in);
        REQUIRE(f);
        std::stringstream ss;
        auto s = streams{f, ss, ss};

        REQUIRE(prompt<int>("input1: ", s) == 30);
        REQUIRE(prompt<int>("input2: ", s) == -20);
        REQUIRE(prompt_min<int>("min: ", 40, s) == 40);
        REQUIRE(prompt_max<int>("max: ", 40, s) == 40);
        REQUIRE(prompt_bounded<int>("bounded: ", 15, 25, s) == 20);

        std::string expected =
            "input1: Please enter an integer (-2147483648 to 2147483647).\n"
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

        REQUIRE(prompt<bool>("inputY: ", s));
        REQUIRE_FALSE(prompt<bool>("inputN: ", s));

        std::string expected =
            "inputY: Please enter (y/n).\n"
            "inputY: inputN: ";

        REQUIRE(ss.str() == expected);

        f.close();
    }
}

} // namespace tools::cli
