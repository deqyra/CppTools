#include <catch2/catch2.hpp>

#include <cpptools/utility/iterable_tools.hpp>

#define TAGS "[iterable]"

namespace tools
{
namespace Iterable
{

TEST_CASE("Max iterable length", TAGS)
{
    std::vector<std::vector<int>> vecs = {
        std::vector<int>(3),
        std::vector<int>(5),
        std::vector<int>(4),
        std::vector<int>(10),
        std::vector<int>(7)
    };

    REQUIRE(max_iterable_length(vecs) == (size_t)10);
}

TEST_CASE("Sum 2nd-order nested iterables", TAGS)
{
    std::vector<std::vector<int>> vectors;
    vectors.push_back({3, 5, 2});
    vectors.push_back({1, 4, 6});
    vectors.push_back({2, 7});
    vectors.push_back({9, 12, 5, 8});

    REQUIRE(sum2nested_iterables<int>(vectors) == 64);
}

TEST_CASE("Index of max element can be found within a vectors and arrays alike", TAGS)
{
    int arr[7] = {2, 9, 8, 11, 4, 17, 7};
    std::vector<int> vec(arr, arr + sizeof(arr) / sizeof(int));

    SECTION("Int vector")
    {
        REQUIRE(index_of_max_element(vec) == 5);
        vec[3] = 100;
        REQUIRE(index_of_max_element(vec) == 3);
    }

    SECTION("Int array")
    {
        REQUIRE(index_of_max_element(arr, 7) == 5);
        arr[3] = 100;
        REQUIRE(index_of_max_element(arr, 7) == 3);
    }
}

TEST_CASE("Elements can be stripped from a vector", TAGS)
{
    std::vector<int> vec = std::vector<int>({0, 2, 854, 0, -123, 2759, 0});
    std::vector<int> ref = std::vector<int>({2, 854, -123, 2759});

    // Make sure it works.
    strip_element_from_iterable(vec, 0);
    REQUIRE(vec == ref);

    // Make sure it is a no-op when no element is present.
    strip_element_from_iterable(vec, 0);
    REQUIRE(vec == ref);
}

} // namespace Iterable
<<<<<<< HEAD:tests/utility/test_iterable_tools.cpp
} // namespace tools
