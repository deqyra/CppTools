#ifndef CPPTOOLS_UTILITY_PREDICATE_HPP
#define CPPTOOLS_UTILITY_PREDICATE_HPP

#include <concepts>

namespace tools
{

template<typename F, typename T>
concept predicate = requires(F& f, const T& v) {
    { f(v) } -> std::same_as<bool>;
};

namespace pred
{

template<typename T, predicate<T> P>
decltype(auto) logical_not(P&& pred) {
    return [pred] (const T& element) -> bool {
        return !pred(element);
    };
}

template<typename T, predicate<T> P1, predicate<T> P2>
decltype(auto) logical_and(P1&& first, P2&& second) {
    return [first, second] (const T& element) -> bool {
        return first(element) && second(element);
    };
}

template<typename T, predicate<T> P1, predicate<T> P2>
decltype(auto) logical_or(P1&& first, P2&& second) {
    return [first, second] (const T& element) -> bool {
        return first(element) || second(element);
    };
}

template<typename T>
decltype(auto) greater_than(const T& value) {
    return [value] (const T& element) -> bool {
        return element > value;
    };
}

template<typename T>
decltype(auto) greater_equal(const T& value) {
    return [value] (const T& element) -> bool {
        return element >= value;
    };
}

template<typename T>
decltype(auto) less_than(const T& value) {
    return [value] (const T& element) -> bool {
        return element < value;
    };
}

template<typename T>
decltype(auto) less_equal(const T& value) {
    return [value] (const T& element) -> bool {
        return element <= value;
    };
}

template<typename T>
decltype(auto) between(const T& low, const T& high) {
    return logical_and<T>(greater_equal<T>(low), less_equal<T>(high));
}

template<typename T>
decltype(auto) strictly_between(const T& low, const T& high) {
    return logical_and<T>(greater_than<T>(low), less_than<T>(high));
}

template<typename T>
decltype(auto) outside(const T& low, const T& high) {
    return logical_not<T>(between<T>(low, high));
}

} // namespace pred

} // namespace tools

#endif//CPPTOOLS_UTILITY_PREDICATE_HPP