#ifndef CPPTOOLS__UTILITY__PREDICATE_HPP
#define CPPTOOLS__UTILITY__PREDICATE_HPP

#include <functional>

namespace tools::predicate
{

template<typename T>
using pred_t = bool (*)(const T&);

template<typename T>
pred_t<T> logical_not(pred_t<T> pred)
{
    return [pred] (const T& element) -> bool
    {
        return !pred(element);
    };
}

template<typename T>
pred_t<T> logical_and(pred_t<T> first, pred_t<T> second)
{
    return [first, second] (const T& element) -> bool
    {
        return first(element) && second(element);
    };
}

template<typename T>
pred_t<T> logical_or(pred_t<T> first, pred_t<T> second)
{
    return [first, second] (const T& element) -> bool
    {
        return first(element) || second(element);
    };
}

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
    return logical_and(greater_equal(low), less_equal(high));
}

template<typename T>
pred_t<T> strictly_between(const T& low, const T& high)
{
    return logical_and(greater_than(low), less_than(high));
}

template<typename T>
pred_t<T> outside(const T& low, const T& high)
{
    return logical_not(between(low, high));
}

} // namespace tools::predicate

#endif//CPPTOOLS__UTILITY__PREDICATE_HPP