#ifndef CPPTOOLS__UTILITY__MERGING_STRATEGY_HPP
#define CPPTOOLS__UTILITY__MERGING_STRATEGY_HPP

#include <concepts>
#include <type_traits>

#include "type_utils.hpp"

namespace tools
{

/// @brief Concept for function-like types that implement strategies to merge
/// two entities of the same type.
template<typename merger_t, typename T>
concept merging_strategy = requires (merger_t m, T& original, T& other)
{
    { m(original, other) } -> std::same_as<void>;
};

template<typename merge_t, typename value_t>
struct is_noexcept_merging_strategy
{
    static constexpr bool value = std::is_nothrow_invocable_v<merge_t, value_t&, const value_t&>;
};

template<typename merge_t, typename value_t>
constexpr bool is_noexcept_merging_strategy_v = is_noexcept_merging_strategy<merge_t, value_t>::value;

/// @brief Collection of merging strategy implementations.
namespace merging_strategies
{

/// @brief Keep the original value and do nothing with the other.
struct keep_original
{
    template<typename T>
    void operator()(T& original, const T& other) {}
};

/// @brief Replace the original value by assigning the other to it.
struct replace_original
{
    template<typename T>
    void operator()(T& original, const T& other)
    {
        original = other;
    }
};

/// @brief Replace the original value by move-assigning the other o it.
struct move_replace_original
{
    template<typename T>
    void operator()(T& original, T& other)
    {
        original = std::move(other);
    }
};

}

} // namespace tools

#endif//CPPTOOLS__UTILITY__MERGING_STRATEGY_HPP