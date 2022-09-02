#ifndef CPPTOOLS__UTILITY__RANGES_HPP
#define CPPTOOLS__UTILITY__RANGES_HPP

#include <algorithm>
#include <concepts>
#include <numeric>
#include <ranges>

#include "concepts.hpp"

namespace tools::ranges
{

/// @brief Find the size of the biggest range in a set of ranges
/// 
/// @tparam ArgTypes Types of the ranges to be passed in as arguments
///
/// @param ranges Different ranges whose size to compare
///
/// @return The size of the biggest range in the provided parameters
template<std::ranges::sized_range... ArgTypes>
size_t max_size(ArgTypes&&... ranges)
{
    return std::max(std::ranges::size(ranges)...);
}

/// @brief Find the size of the biggest range in a range of ranges
/// 
/// @tparam R Type of the range of nested ranges
/// @tparam N Type of the nested ranges
///
/// @param range Range of nested ranges
///
/// @return The size of the biggest range in the range of ranges
template<std::ranges::range R, std::ranges::sized_range N>
size_t nested_max_size(const R& range)
{
    auto get_size = [](const N& nested_range) -> size_t
    {
        return std::ranges::size(nested_range);
    };

    return std::ranges::max(range | std::views::transform(get_size));
}

/// @brief Sum all elements in a range of nested ranges
/// 
/// @tparam R Type of the range of nested ranges
/// @tparam N Type of the nested ranges
/// @tparam T Type of the elements to sum
///
/// @param range Range of nested ranges
///
/// @return The sum of all elements in the nested range
template<std::ranges::range R, std::ranges::range N, typename T>
requires concepts::addable<T>
T nested_sum(const R& range)
{
    auto do_sum = [](const N& inner_range) -> T
    {
        return std::accumulate(
            std::cbegin(inner_range),
            std::cend(inner_range),
            declval<T>()
        );
    };

    auto view = range | std::views::transform(do_sum);
    return std::accumulate(
        std::cbegin(view),
        std::cend(view),
        declval<T>()
    );
}

} // namespace tools::ranges

#endif//CPPTOOLS__UTILITY__RANGES_HPP