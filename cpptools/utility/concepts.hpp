#ifndef CPPTOOLS__UTILITY__CONCEPTS_HPP
#define CPPTOOLS__UTILITY__CONCEPTS_HPP

#include <type_traits>

namespace tools
{

/// @brief Satisfied by any type on which a \c to_string function can be called.
/// Such a function must be visible (possibly through ADL) at the point of usage
/// of this concept.
template<typename T>
concept stringable = requires (T v) {
    to_string(v);
};

/// @brief Satisfied if T and U are the same types, notwithstanding cv- and
/// reference-qualifiers
template<typename T, typename U>
concept any_cvref = std::is_same_v<std::remove_cvref_t<U>, std::remove_cvref_t<T>>;

/// @brief Satisfied if E is an enumeration type
template<typename E>
concept enumeration = std::is_enum_v<E>;

} // namespace tools

#endif//CPPTOOLS__UTILITY__CONCEPTS_HPP