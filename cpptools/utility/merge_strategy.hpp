#ifndef CPPTOOLS__UTILITY__MERGE_STRATEGY_HPP
#define CPPTOOLS__UTILITY__MERGE_STRATEGY_HPP

#include <concepts>
#include <type_traits>

namespace tools
{

/// @brief Satisfied by any stateless function-like type that takes in two
/// entities of the same type and processes them in some way
template<typename merge_t, typename T>
concept merge_strategy = requires (merge_t m, T& original, T& other) {
    { m(original, other) } -> std::same_as<void>;
};

template<typename merge_t, typename value_t> requires merge_strategy<merge_t, value_t>
struct is_noexcept_merge_strategy {
    static constexpr bool value = std::is_nothrow_invocable_v<merge_t, value_t&, const value_t&>;
};

template<typename merge_t, typename value_t>
constexpr bool is_noexcept_merge_strategy_v = is_noexcept_merge_strategy<merge_t, value_t>::value;

/// @brief Collection of merge strategy implementations
namespace merge
{

/// @brief Keep the original value and do nothing with the other
struct keep {
    template<typename T>
    void operator()(T& original, const T& other) {}
};

/// @brief Replace the original value by assigning the other to it
struct copy_replace {
    template<typename T>
    void operator()(T& original, const T& other) {
        original = other;
    }
};

/// @brief Replace the original value by move-assigning the other to it
struct move_replace {
    template<typename T>
    void operator()(T& original, T& other) {
        original = std::move(other);
    }
};

} // namespace merge

} // namespace tools

#endif//CPPTOOLS__UTILITY__MERGE_STRATEGY_HPP