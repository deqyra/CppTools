#ifndef CPPTOOLS_UTILITY_TYPE_TRAITS_HPP
#define CPPTOOLS_UTILITY_TYPE_TRAITS_HPP

#include <iterator>
#include <type_traits>

namespace tools {

namespace detail {
    
    constexpr inline bool _const = true;
    constexpr inline bool _not_const = false;

}

template<bool Const, typename T>
using maybe_const = std::conditional_t<Const, const T, T>;

template<typename It>
using category_of = typename std::iterator_traits<It>::iterator_category;

} // namespace tools

#endif//CPPTOOLS_UTILITY_TYPE_TRAITS_HPP