#ifndef TOOLS__UTILITY__BITWISE_ENUM_HPP
#define TOOLS__UTILITY__BITWISE_ENUM_HPP

#include <concepts>
#include <type_traits>

namespace tools
{
    
template<typename T>
struct enable_bitwise_enum : public std::false_type { };

template<typename T>
static constexpr bool enable_bitwise_enum_v = enable_bitwise_enum<T>::value;

template<typename T>
concept bitwise_enabled_enum = std::is_enum_v<T> && enable_bitwise_enum_v<T>;

namespace bitwise_enum_ops
{

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    T operator&(const T& left, const T& right)
    {
        return (T)((i)left & (i)right);
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    T& operator&=(T& left, const T& right)
    {
        return left = (T)((i)left & (i)right);
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    T operator|(const T& left, const T& right)
    {
        return (T)((i)left | (i)right);
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    T& operator|=(T& left, const T& right)
    {
        return left = (T)((i)left | (i)right);
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    T operator^(const T& left, const T& right)
    {
        return (T)((i)left ^ (i)right);
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    T& operator^=(T& left, const T& right)
    {
        return left = (T)((i)left ^ (i)right);
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    T operator~(const T& value)
    {
        return (T)(~(i)value);
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    bool none(const T& value)
    {
        return (i)value == (i)0;
    }

    template<bitwise_enabled_enum T, typename i = std::underlying_type_t<const T>>
    bool any(const T& value)
    {
        return !none(value);
    }

} // namespace bitwise_enum_ops

} // namespace tools

#endif//TOOLS__UTILITY__BITWISE_ENUM_HPP