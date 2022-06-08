#ifndef TOOLS__UTILITY__LAMBDA_MAKER_HPP
#define TOOLS__UTILITY__LAMBDA_MAKER_HPP

#include <functional>

namespace tools
{
namespace LambdaMaker
{

template<typename T>
std::function<bool(const T&)> greater_than(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element > value;
    };
}

template<typename T>
std::function<bool(const T&)> greater_equal(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element >= value;
    };
}

template<typename T>
std::function<bool(const T&)> less_than(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element < value;
    };
}

template<typename T>
std::function<bool(const T&)> less_equal(const T& value)
{
    return [value] (const T& element) -> bool
    {
        return element <= value;
    };
}

template<typename T>
std::function<bool(const T&)> between(const T& low, const T& high)
{
    return [low, high] (const T& element) -> bool
    {
        return element >= low && element <= high;
    };
}

template<typename T>
std::function<bool(const T&)> strictly_between(const T& low, const T& high)
{
    return [low, high] (const T& element) -> bool
    {
        return element > low && element < high;
    };
}

} // namespace LambdaMaker
} // namespace tools

#endif//TOOLS__UTILITY__LAMBDA_MAKER_HPP