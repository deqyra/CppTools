#ifndef CPPTOOLS__UTILITY__TYPE_UTILS_HPP
#define CPPTOOLS__UTILITY__TYPE_UTILS_HPP

#include <type_traits>
#include <utility>

namespace tools
{

template<typename T, typename ...Ts>
struct is_one_of
{
    static constexpr bool value = (std::is_same_v<T, Ts> | ...);
};

template<typename T, typename ...Ts>
constexpr bool is_one_of_v = is_one_of<T, Ts...>::value;

}

#include "type_utils_impl/type_list.hpp"

#endif//CPPTOOLS__UTILITY__TYPE_UTILS_HPP