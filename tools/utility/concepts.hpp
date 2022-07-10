#ifndef TOOLS__UTILITY__CONCEPTS_HPP
#define TOOLS__UTILITY__CONCEPTS_HPP

#include <type_traits>
namespace tools
{

template<typename T>
concept integral_type = requires { std::is_integral_v<T>; };


} // namespace tools

#endif//TOOLS__UTILITY__CONCEPTS_HPP