#ifndef CPPTOOLS_UTILITY_PREDICATE_HPP
#define CPPTOOLS_UTILITY_PREDICATE_HPP

#include <concepts>

#include <cpptools/utility/deduce_parameters.hpp>

namespace tools {

template<typename F, typename T>
concept predicate = requires(F& f, const T& v) {
    { f(v) } -> std::same_as<bool>;
};

namespace pred {

template<typename T, predicate<T> P>
decltype(auto) negation(P&& pred) {
    return [pred] (const T& element) -> bool {
        return !pred(element);
    };
}

template<typename T, predicate<T> P1, predicate<T> P2>
decltype(auto) conjunction(P1&& first, P2&& second) {
    return [first, second] (const T& element) -> bool {
        return first(element) && second(element);
    };
}

template<typename T, predicate<T> P1, predicate<T> P2>
decltype(auto) disjunction(P1&& first, P2&& second) {
    return [first, second] (const T& element) -> bool {
        return first(element) || second(element);
    };
}

template<typename T>
decltype(auto) equals(const T& value) {
    return [value] (const T& element) -> bool {
        return element == value;
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
    return conjunction<T>(greater_equal<T>(low), less_equal<T>(high));
}

template<typename T>
decltype(auto) strictly_between(const T& low, const T& high) {
    return conjunction<T>(greater_than<T>(low), less_than<T>(high));
}

template<typename T>
decltype(auto) outside_of(const T& low, const T& high) {
    return negation<T>(between<T>(low, high));
}

template<typename P>
decltype(auto) first_member(P&& pred) {
    return [pred] (auto pair) -> bool {
        return pred(pair.first);
    };
}

template<typename P>
decltype(auto) second_member(P&& pred) {
    return [pred] (auto pair) -> bool {
        return pred(pair.second);
    };
}

} // namespace pred

} // namespace tools

#endif//CPPTOOLS_UTILITY_PREDICATE_HPP