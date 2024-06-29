#ifndef CPPTOOLS_UTILITY_UNARY_HPP
#define CPPTOOLS_UTILITY_UNARY_HPP

#include <utility>

namespace tools::unary {

template<typename T>
decltype(auto) dereference(T&& v) {
    return *v;
}

template<typename T, typename U>
std::pair<U, T> reverse_pair(const std::pair<T, U>& p) {
    return std::make_pair(p.second, p.first);
}

template<typename T, typename U>
std::pair<U, T> reverse_pair(std::pair<T, U>&& p) {
    return std::make_pair(std::move(p.second), std::move(p.first));
}

} // namespace tools::unary

#endif//CPPTOOLS_UTILITY_UNARY_HPP