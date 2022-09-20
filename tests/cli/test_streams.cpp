#include <catch2/catch2.hpp>

#include <iostream>
#include <fstream>

#include <cpptools/cli/streams.hpp>

#define TAGS "[cli][streams]"

namespace tools::cli
{

TEST_CASE("CLI stream wrapper", TAGS)
{
    streams s{std::cin, std::cout, std::cerr};
    REQUIRE_NOTHROW(streams{std::cin, std::cout, std::cerr});

    // Encapsulated streams write to and read from the same buffers as those provided
    REQUIRE(s.in.rdbuf() == std::cin.rdbuf());
    REQUIRE(s.out.rdbuf() == std::cout.rdbuf());
    REQUIRE(s.err.rdbuf() == std::cerr.rdbuf());
}

} // namespace tools::cli
