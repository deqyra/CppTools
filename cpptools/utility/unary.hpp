#ifndef CPPTOOLS_UTILITY_UNARY_HPP
#define CPPTOOLS_UTILITY_UNARY_HPP

#include <utility>

namespace tools {

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

template<typename P>
auto reverse_map_pair(P&& p) {
    using pair_in_t  = std::remove_cvref_t<P>;
    using T1 = typename pair_in_t::first_type;
    using T2 = typename pair_in_t::second_type;
    using pair_out_t = std::pair<const T2, std::remove_const_t<T1>>;
    
    return pair_out_t{ std::forward_like<P>(p.second), std::forward_like<P>(p.first) };
}

} // namespace tools

#endif//CPPTOOLS_UTILITY_UNARY_HPP