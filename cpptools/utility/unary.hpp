#ifndef CPPTOOLS_UTILITY_UNARY_HPP
#define CPPTOOLS_UTILITY_UNARY_HPP

#include <type_traits>
#include <utility>

namespace tools {

template<typename T>
[[nodiscard]] constexpr decltype(auto) dereference(T&& v)
    noexcept(noexcept(*v))
{
    return *v;
}

template<typename T, typename U>
[[nodiscard]] constexpr std::pair<U, T> reverse_pair(const std::pair<T, U>& p)
    noexcept(noexcept(std::make_pair(p.second, p.first)))
{
    return std::make_pair(p.second, p.first);
}

template<typename T, typename U>
[[nodiscard]] constexpr std::pair<U, T> reverse_pair(std::pair<T, U>&& p)
    noexcept(noexcept(std::make_pair(std::move(p.second), std::move(p.first))))
{
    return std::make_pair(std::move(p.second), std::move(p.first));
}

template<typename P>
using reversed_map_pair = std::pair<
    const typename std::remove_cvref_t<P>::second_type,
    std::remove_const_t<typename std::remove_cvref_t<P>::first_type>
>;

template<typename P>
[[nodiscard]] constexpr auto reverse_map_pair(P&& p)
    noexcept(std::is_nothrow_constructible_v<reversed_map_pair<P>, decltype(std::forward_like<P>(p.second)), decltype(std::forward_like<P>(p.first))>)
{
    return reversed_map_pair<P>{ std::forward_like<P>(p.second), std::forward_like<P>(p.first) };
}

} // namespace tools

#endif//CPPTOOLS_UTILITY_UNARY_HPP