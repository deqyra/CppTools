#include <catch2/catch2.hpp>

#include <vector>
#include <algorithm>
#include <functional>

#include <cpptools/lambda_maker.hpp>

#define TAGS "[tools][lambda_maker]"

namespace LambdaMakerTest
{
    std::vector<int> values = {15, 3, 21, 14, 5, 9, 0};
    std::vector<bool> result = std::vector<bool>(values.size());

    void testLambda(std::function<bool(const int&)> lambda)
    {
        std::transform(values.begin(), values.end(), result.begin(), lambda);
    }

    TEST_CASE("Test greater_than", TAGS)
    {
        std::vector<bool> expected = {true, false, true, true, false, false, false};
        testLambda(LambdaMaker::greater_than(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test greater_equal", TAGS)
    {
        std::vector<bool> expected = {true, false, true, true, false, true, false};
        testLambda(LambdaMaker::greater_equal(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test less_than", TAGS)
    {
        std::vector<bool> expected = {false, true, false, false, true, false, true};
        testLambda(LambdaMaker::less_than(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test less_equal", TAGS)
    {
        std::vector<bool> expected = {false, true, false, false, true, true, true};
        testLambda(LambdaMaker::less_equal(9));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test between", TAGS)
    {
        std::vector<bool> expected = {false, false, false, true, false, true, false};
        testLambda(LambdaMaker::between(9, 14));
        REQUIRE(result == expected);
    }

    TEST_CASE("Test strictly_between", TAGS)
    {
        std::vector<bool> expected = {false, false, false, false, false, false, false};
        testLambda(LambdaMaker::strictly_between(9, 14));
        REQUIRE(result == expected);
    }
}
