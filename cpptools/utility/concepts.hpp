#ifndef CPPTOOLS__UTILITY__CONCEPTS_HPP
#define CPPTOOLS__UTILITY__CONCEPTS_HPP

#include <concepts>
#include <type_traits>

#include <cpptools/utility/to_string.hpp>

namespace tools
{

template<typename T>
concept stringable = requires (T v) {
    to_string(v);
};

} // namespace tools

#endif//CPPTOOLS__UTILITY__CONCEPTS_HPP