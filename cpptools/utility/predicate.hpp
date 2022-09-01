#ifndef TOOLS__UTILITY__PREDICATE_HPP
#define TOOLS__UTILITY__PREDICATE_HPP

#include <functional>

namespace tools::predicate
{

template<typename T>
using pred_t = bool(const T&);

template<typename T>
pred_t<T> greater_than(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element > value;
    };
}

template<typename T>
pred_t<T> greater_equal(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element >= value;
    };
}

template<typename T>
pred_t<T> less_than(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element < value;
    };
}

template<typename T>
pred_t<T> less_equal(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element <= value;
    };
}

template<typename T>
pred_t<T> between(const T& low, const T& high)
{
    return [low, high] (const T& element) -> bool
    {
        return element >= low && element <= high;
    };
}

template<typename T>
pred_t<T> strictly_between(const T& low, const T& high)
{
    return [low, high] (const T& element) -> bool
    {
        return element > low && element < high;
    };
}

} // namespace tools::predicate

#endif//TOOLS__UTILITY__PREDICATE_HPP