#include <catch2/catch2.hpp>

#include <iostream>
#include <fstream>

#include <cpptools/cli/cli_streams.hpp>

#define TAGS "[cli][cli_streams]"

namespace tools
{

TEST_CASE("CLI stream wrapper", TAGS)
{
    cli_streams streams = cli_streams(std::cin, std::cout, std::cerr);
    REQUIRE_NOTHROW(cli_streams(std::cin, std::cout, std::cerr));

    // Encapsulated streams write to and read from the same buffers as those provided
    REQUIRE(streams.in.rdbuf() == std::cin.rdbuf());
    REQUIRE(streams.out.rdbuf() == std::cout.rdbuf());
    REQUIRE(streams.err.rdbuf() == std::cerr.rdbuf());
}

} // namespace tools
