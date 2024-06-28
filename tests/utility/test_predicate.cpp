#include <catch2/catch_all.hpp>

#include <vector>
#include <algorithm>

#include <cpptools/utility/predicate.hpp>

#define TAGS "[predicate]"

namespace tools::pred
{
    std::vector<int> values = {15, 3, 21, 14, 5, 9, 0};
    std::vector<bool> result = std::vector<bool>(values.size());

    template<predicate<int> P>
    void test_lambda(P&& pred)
    {
        std::transform(values.begin(), values.end(), result.begin(), pred);
    }

    TEST_CASE("Test greater_than", TAGS)
    {
        std::vector<bool> expected = {true, false, true, true, false, false, false};
        test_lambda(greater_than(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test greater_equal", TAGS)
    {
        std::vector<bool> expected = {true, false, true, true, false, true, false};
        test_lambda(greater_equal(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test less_than", TAGS)
    {
        std::vector<bool> expected = {false, true, false, false, true, false, true};
        test_lambda(less_than(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test less_equal", TAGS)
    {
        std::vector<bool> expected = {false, true, false, false, true, true, true};
        test_lambda(less_equal(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test between", TAGS)
    {
        std::vector<bool> expected = {false, false, false, true, false, true, false};
        test_lambda(between(9, 14));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test strictly_between", TAGS)
    {
        std::vector<bool> expected = {false, false, false, false, false, false, false};
        test_lambda(strictly_between(9, 14));
        REQUIRE(result == expected);
    }
} // namespace tools::pred
