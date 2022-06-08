#ifndef TOOLS__UTILITY__TYPE_TRAITS_IMPL__CONTAINER_TRAITS_HPP
#define TOOLS__UTILITY__TYPE_TRAITS_IMPL__CONTAINER_TRAITS_HPP

#include <type_traits>
#include <valarray>
#include <utility>

#include "container_traits_detail.hpp"

namespace type_utils
{

template <typename T>
struct is_container : public std::integral_constant<
    bool,
    detail::has_const_iterator<T>::value &&
    detail::has_begin<T>::value &&
    detail::has_end<T>::value
> { };

template <typename T, std::size_t N>
struct is_container<T[N]> : std::true_type { };

template <std::size_t N>
struct is_container<char[N]> : std::false_type { };

template <typename T>
struct is_container<std::valarray<T>> : std::true_type { };

template <typename T1, typename T2>
struct is_container<std::pair<T1, T2>> : std::true_type { };

template <typename ...Args>
struct is_container<std::tuple<Args...>> : std::true_type { };

template <typename T>
struct is_sized_container : public std::integral_constant<
    bool,
    detail::has_const_iterator<T>::value &&
    detail::has_begin<T>::value &&
    detail::has_end<T>::value &&
    detail::has_size<T>::value
> { };

template <typename T, std::size_t N>
struct is_sized_container<T[N]> : std::true_type { };

template <std::size_t N>
struct is_sized_container<char[N]> : std::false_type { };

template <typename T>
struct is_sized_container<std::valarray<T>> : std::true_type { };

template <typename T1, typename T2>
struct is_sized_container<std::pair<T1, T2>> : std::true_type { };

template <typename ...Args>
struct is_sized_container<std::tuple<Args...>> : std::true_type { };

} // namespace type_utils

#endif//TOOLS__UTILITY__TYPE_TRAITS_IMPL_CONTAINER_TRAITSL_HPP