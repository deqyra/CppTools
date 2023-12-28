#ifndef CPPTOOLS___INTERNAL__DEBUG_EXPR_HPP
#define CPPTOOLS___INTERNAL__DEBUG_EXPR_HPP

#include <ranges>

namespace tools::internal::debug_expr
{

namespace detail
{
    template<typename T, typename... Us>
    bool all_null(T ptr, Us... ptrs)
    {
        return ptr == nullptr && all_null(ptrs...);
    }

    template<typename T>
    bool all_null(T ptr)
    {
        return ptr == nullptr;
    }

    template<typename T, typename... Us>
    bool all_not_null(T ptr, Us... ptrs)
    {
        return ptr != nullptr && all_not_null(ptrs...);
    }

    template<typename T>
    bool all_not_null(T ptr)
    {
        return ptr != nullptr;
    }
}

template<typename T>
bool null(T* ptr)
{
    return ptr == nullptr;
}

template<typename T, typename U>
bool both_null(T* left, U* right)
{
    return left == nullptr && right == nullptr;
}

template<typename... Ts>
bool all_null(Ts*... ptrs)
{
    return detail::all_null(ptrs...);
}

template<typename T>
bool not_null(T* ptr)
{
    return ptr != nullptr;
}

template<typename T, typename U>
bool both_not_null(T* left, U* right)
{
    return left != nullptr && right != nullptr;
}

template<typename... Ts>
bool all_not_null(Ts*... ptrs)
{
    return detail::all_not_null(ptrs...);
}

template<typename Range, typename T>
bool in_range(const Range& range, const T& value)
{
    return std::ranges::find(range, value) != std::end(range);
}

template<typename Range, typename T>
bool in_range_keys(const Range& range, const T& value)
{
    return range.find(value) != range.end();
}

template<typename Range, typename T>
bool not_in_range(const Range& range, const T& value)
{
    return !in_range(range, value);
}

template<typename T, typename U>
bool in_range(const U& lower_bound, const T& value, const U& greater_bound)
{
    return (lower_bound <= value) && (value < greater_bound);
}

template<typename T, typename U>
bool between(const U& lower_bound, const T& value, const U& greater_bound)
{
    return (lower_bound <= value) && (value <= greater_bound);
}

template<typename T, typename U>
bool strictly_between(const U& lower_bound, const T& value, const U& greater_bound)
{
    return (lower_bound < value) && (value < greater_bound);
}

} // namespace tools::internal::debug_expr

#endif//CPPTOOLS___INTERNAL__DEBUG_EXPR_HPP