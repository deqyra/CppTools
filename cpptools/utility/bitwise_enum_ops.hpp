#ifndef CPPTOOLS_UTILITY_BITWISE_ENUM_HPP
#define CPPTOOLS_UTILITY_BITWISE_ENUM_HPP

#include <type_traits>

#include "concepts.hpp"

namespace tools
{

/// @brief Controls whether operator overloads from namespace bitwise_enum_ops
/// should participate in overload resolution for enum type E.
///
/// @note Support for bitwise operations on an enum type is opt-in: first
/// specialize this trait and have it inherit from \c std::true_type for your
/// own enum type, and then add a \c{using namespace bitwise_enum_ops;}
/// declaration to use the bitwise operator overloads on enumerators.
template<enumeration E>
struct enable_bitwise_enum : public std::false_type { };

template<enumeration E>
static constexpr bool enable_bitwise_enum_v = enable_bitwise_enum<E>::value;

template<typename E>
concept bitwise_enabled_enum = enable_bitwise_enum_v<E>;

namespace bitwise_enum_ops
{

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
E operator&(const E& left, const E& right) {
    return static_cast<E>(
        static_cast<i>(left) & static_cast<i>(right)
    );
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
E& operator&=(E& left, const E& right) {
    return left = static_cast<E>(
        static_cast<i>(left) & static_cast<i>(right)
    );
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
E operator|(const E& left, const E& right) {
    return static_cast<E>(
        static_cast<i>(left) | static_cast<i>(right)
    );
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
E& operator|=(E& left, const E& right) {
    return left = static_cast<E>(
        static_cast<i>(left) | static_cast<i>(right)
    );
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
E operator^(const E& left, const E& right) {
    return static_cast<E>(
        static_cast<i>(left) ^ static_cast<i>(right)
    );
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
E& operator^=(E& left, const E& right) {
    return left = static_cast<E>(
        static_cast<i>(left) ^ static_cast<i>(right)
    );
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
E operator~(const E& value) {
    return static_cast<E>(~static_cast<i>(value));
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
bool none(const E& value) {
    return static_cast<i>(value) == static_cast<i>(0);
}

template<bitwise_enabled_enum E, typename i = std::underlying_type_t<E>>
bool any(const E& value) {
    return !none(value);
}

} // namespace bitwise_enum_ops

} // namespace tools

#endif//CPPTOOLS_UTILITY_BITWISE_ENUM_HPP