#ifndef CPPTOOLS_UTILITY_WRAPPING_RANGE_HPP
#define CPPTOOLS_UTILITY_WRAPPING_RANGE_HPP

#include <cmath>
#include <concepts>

#include "concepts.hpp"

namespace tools {

/// @brief Wrapper which makes its contained value wrap around a given range 
/// whenever it should exceed either bound of it
template<arithmetic T, T Min, T ExMax>
class wrapping_value {
    static_assert(Min <  ExMax, "Min cannot be greater than Max");
    static_assert(Min != ExMax, "Min and Max cannot be equal");

    static inline constexpr auto Range = ExMax - Min;

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

    template<typename V> requires std::integral<T>
    [[nodiscard]] static constexpr T _constrain(V v) {
        using target_t = _int_or_fp<V>::type;

        auto rangeMod = (static_cast<target_t>(v) - Min) % Range;
        return static_cast<T>(rangeMod + ((rangeMod < 0) ? ExMax : Min));
    }

    template<typename V> requires std::floating_point<T>
    [[nodiscard]] static constexpr T _constrain(V v) {
        using target_t = _int_or_fp<V>::type;

        auto rangeMod = std::fmod(static_cast<target_t>(v) - Min, Range);
        return static_cast<T>(rangeMod + ((rangeMod < 0) ? ExMax : Min));
    }

public:
    template<typename V>
    wrapping_value(V v) :
        _value(_constrain(v))
    {
        
    }

    wrapping_value(const wrapping_value&) = default;
    wrapping_value(wrapping_value&&) = default;
    wrapping_value& operator=(const wrapping_value&) = default;
    wrapping_value& operator=(wrapping_value&&) = default;

    operator const T&() const {
        return _value;
    }

    template<typename V>
    wrapping_value& operator=(V v) {
        _value = _constrain(v);

        return *this;
    }

    template<typename V>
    wrapping_value& operator+=(V v) {
        _value = _constrain(_value + v);

        return *this;
    }

    template<typename V>
    wrapping_value& operator-=(V v) {
        _value = _constrain(_value - v);

        return *this;
    }

    template<typename V>
    [[nodiscard]] constexpr friend bool operator==(const wrapping_value& left, V right) {
        return left._value == right;
    }

    template<typename V>
    [[nodiscard]] constexpr friend bool operator==(V left, const wrapping_value& right) {
        return right == left;
    }

    template<typename V>
    [[nodiscard]] constexpr friend wrapping_value operator+(const wrapping_value& left, V right) {
        return left._value + right;
    }

    template<typename V>
    [[nodiscard]] constexpr friend wrapping_value operator+(V left, const wrapping_value& right) {
        return right + left;
    }

    template<typename V>
    [[nodiscard]] constexpr friend wrapping_value operator-(const wrapping_value& left, V right) {
        return left._value - right;
    }

    template<typename V>
    [[nodiscard]] constexpr friend wrapping_value operator-(V left, const wrapping_value& right) {
        return right - left;
    }
};

} // namespace tools

#endif//CPPTOOLS_UTILITY_WRAPPING_RANGE_HPP