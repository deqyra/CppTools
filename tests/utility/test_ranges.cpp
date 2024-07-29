#include <catch2/catch_all.hpp>

#include <array>

#include <cpptools/utility/ranges.hpp>

#define TAGS "[ranges]"

namespace tools::ranges::test {

TEST_CASE("dereference_view", TAGS) {
    struct foo {
        int a;
        int b;

        int operator*() const {
            return a * b;
        }
    };

    std::vector<foo> v = {
        { .a =  2, .b = 3 },
        { .a =  4, .b = 5 },
        { .a = -8, .b = 8 }
    };

    REQUIRE(std::ranges::equal(v | views::dereference, std::to_array({6, 20, -64})));
    REQUIRE(std::ranges::equal(ranges::dereference_view(v), std::to_array({6, 20, -64})));
}

TEST_CASE("reverse_pair_view", TAGS) {
    using pair = std::pair<int, float>;
    using reversed = std::pair<float, int>;

    std::vector<pair> v = {
        {  2, 3.5f },
        {  4, 5.5f },
        { -8, 8.5f }
    };

    std::vector<reversed> expected = {
        { 3.5f,  2 },
        { 5.5f,  4 },
        { 8.5f, -8 }
    };

    REQUIRE(std::ranges::equal(v | views::reverse_pair,      expected));
    REQUIRE(std::ranges::equal(ranges::reverse_pair_view(v), expected));
}

TEST_CASE("reverse_map_pair_view", TAGS) {
    using map = std::map<int, float>;
    using reversed_map = std::map<float, int>;

    map v = {
        {  2, 3.5f },
        {  4, 5.5f },
        { -8, 8.5f }
    };

    reversed_map expected = {
        { 3.5f,  2 },
        { 5.5f,  4 },
        { 8.5f, -8 }
    };

    REQUIRE(std::ranges::equal(     v | views::reverse_map_pair | std::ranges::to<std::map>(), expected));
    REQUIRE(std::ranges::equal(ranges::reverse_map_pair_view(v) | std::ranges::to<std::map>(), expected));
}

TEST_CASE("member_view", TAGS) {
    struct foo {
        int a;
        int b;
    };

    std::vector<foo> v = {
        { .a =  2, .b = 3 },
        { .a =  4, .b = 5 },
        { .a = -8, .b = 8 }
    };

    REQUIRE(std::ranges::equal(v | views::member(&foo::b),      std::to_array({3, 5, 8})));
    REQUIRE(std::ranges::equal(ranges::member_view(v, &foo::b), std::to_array({3, 5, 8})));
}

}