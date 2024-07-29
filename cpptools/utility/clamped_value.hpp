#ifndef CPPTOOLS_UTILITY_CLAMPED_VALUE_HPP
#define CPPTOOLS_UTILITY_CLAMPED_VALUE_HPP

#include <algorithm>
#include <type_traits>

#include <cpptools/utility/concepts.hpp>

namespace tools {

/// @brief Wrapper which prevents its contained value from exceeding either
/// bound of a given range
template<arithmetic T, T Min, T Max>
class clamped_value {
    static_assert(Min <  Max, "Min cannot be greater than Max");
    static_assert(Min != Max, "Min and Max cannot be equal");

    T _value;

    template<typename>
    struct _int_or_fp {};

    template<std::integral Int>
    struct _int_or_fp<Int> {
        using type = std::conditional_t<std::integral<T>, Int, T>;
    };

    template<std::floating_point Float>
    struct _int_or_fp<Float> {
        using type = std::conditional_t<std::integral<T>, T, Float>;
    };

    template<typename V>
    [[nodiscard]] static constexpr T _constrain(V v) {
        using target_t = _int_or_fp<V>::type;
        using wider_t = std::conditional_t<(sizeof(T) < sizeof(target_t)), target_t, T>;
        
        return static_cast<T>(std::clamp(static_cast<wider_t>(v), static_cast<wider_t>(Min), static_cast<wider_t>(Max)));
    }

public:
    template<typename V>
    constexpr clamped_value(V v) :
        _value(_constrain(v))
    {
        
    }

    constexpr clamped_value(const clamped_value&) = default;
    constexpr clamped_value(clamped_value&&) = default;
    constexpr clamped_value& operator=(const clamped_value&) = default;
    constexpr clamped_value& operator=(clamped_value&&) = default;

    template<typename V>
    constexpr clamped_value& operator=(V v) noexcept {
        _value = _constrain(v);

        return *this;
    }

    template<typename V>
    constexpr clamped_value& operator+=(V v) noexcept {
        _value = _constrain(_value + v);

        return *this;
    }

    template<typename V>
    constexpr clamped_value& operator-=(V v) noexcept {
        _value = _constrain(_value - v);

        return *this;
    }

    template<typename V>
    [[nodiscard]] constexpr friend bool operator==(const clamped_value& left, V right) {
        return left._value == right;
    }

    template<typename V>
    [[nodiscard]] constexpr friend bool operator==(V left, const clamped_value& right) {
        return right == left;
    }

    template<typename V>
    [[nodiscard]] constexpr friend clamped_value operator+(const clamped_value& left, V right) {
        return left._value + right;
    }

    template<typename V>
    [[nodiscard]] constexpr friend clamped_value operator+(V left, const clamped_value& right) {
        return right + left;
    }

    template<typename V>
    [[nodiscard]] constexpr friend clamped_value operator-(const clamped_value& left, V right) {
        return left._value - right;
    }

    template<typename V>
    [[nodiscard]] constexpr friend clamped_value operator-(V left, const clamped_value& right) {
        return right - left;
    }
};

} // namespace tools

#endif//CPPTOOLS_UTILITY_CLAMPED_VALUE_HPP