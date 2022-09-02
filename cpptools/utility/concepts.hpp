#ifndef CPPTOOLS__UTILITY__CONCEPTS_HPP
#define CPPTOOLS__UTILITY__CONCEPTS_HPP

#include <concepts>
#include <type_traits>

namespace tools::concepts
{

template<typename T>
concept integral_type = requires { std::is_integral_v<T>; };

template<typename T>
concept addable = requires (T a, T b) { {a + b} -> std::convertible_to<T>; };

} // namespace tools

#endif//CPPTOOLS__UTILITY__CONCEPTS_HPP