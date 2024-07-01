#ifndef CPPTOOLS_UTILITY_CONCEPTS_HPP
#define CPPTOOLS_UTILITY_CONCEPTS_HPP

#include <concepts>
#include <type_traits>

namespace tools {

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

/// @brief Satisfied if T is an arithmetic type
template<typename T>
concept arithmetic = std::is_arithmetic_v<T>;

/// @brief Satisfied if T is a numeric type
template<typename T>
concept numeric = arithmetic<T> || enumeration<T>;

template<typename T>
concept implicit_lifetime_class = std::is_trivially_destructible_v<std::remove_cv_t<T>> &&
    (std::is_trivially_default_constructible_v<std::remove_cv_t<T>> || std::is_aggregate_v<std::remove_cv_t<T>>);

template<typename T>
struct is_implicit_lifetime_class : std::bool_constant<false> {};

template<implicit_lifetime_class T>
struct is_implicit_lifetime_class<T> : std::bool_constant<true> {};

template<typename T>
constexpr inline bool is_implicit_lifetime_class_v = is_implicit_lifetime_class<T>::value;

template<typename T>
concept implicit_lifetime_type = std::is_scalar_v<std::remove_cv_t<T>>
    || is_implicit_lifetime_class_v<std::remove_cv_t<T>>
    || std::is_array_v<std::remove_cv_t<T>>;

template<typename T>
struct is_implicit_lifetime_type : std::bool_constant<false> {};

template<implicit_lifetime_type T>
struct is_implicit_lifetime_type<T> : std::bool_constant<true> {};

template<typename T>
constexpr inline bool is_implicit_lifetime_type_v = is_implicit_lifetime_type<T>::value;

template<typename T>
concept array_type = std::is_array_v<T>;

template<typename T, typename E>
concept array_of = array_type<T> && std::is_same_v<E, std::remove_all_extents<T>>;

} // namespace tools

#endif//CPPTOOLS_UTILITY_CONCEPTS_HPP