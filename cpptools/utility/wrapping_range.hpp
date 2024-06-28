#ifndef CPPTOOLS_UTILITY_WRAPPING_RANGE_HPP
#define CPPTOOLS_UTILITY_WRAPPING_RANGE_HPP

#include <cmath>
#include <concepts>

#include "concepts.hpp"

namespace tools {

/// @brief Wrapper which makes its contained value wrap around whenever it should
/// exceed whichever bound of the range it represents
template<arithmetic T, T Min, T Max>
class wrapping_range {
    static_assert(Min <  Max, "Min cannot be greater than Max");
    static_assert(Min != Max, "Min and Max cannot be equal");

    static inline constexpr T Zero = static_cast<T>(0);
    static inline constexpr T Range = Max - Min;
    static inline constexpr bool RootedAtZero = (Max == Zero || Min == Zero);

    T _value;

public:
    wrapping_range(T v) :
        _value(v)
    {
        
    }

    wrapping_range(const wrapping_range&) = default;
    wrapping_range(wrapping_range&&) = default;
    wrapping_range& operator=(const wrapping_range&) = default;
    wrapping_range& operator=(wrapping_range&&) = default;

    operator const T&() const {
        return _value;
    }

    template<typename V>
    wrapping_range& operator=(V v) {
        if constexpr (std::integral<T>) {
            _value = ((v - Min) % Range) + Min;
        } else {
            _value = std::fmod(v - Min, Range) + Min;
        }

        return *this;
    }

    template<typename V>
    wrapping_range& operator+=(V v) {
        _value += v;
        if (_value > Max) {
            _value -= Range;
        }

        return *this;
    }

    template<typename V>
    wrapping_range& operator-=(V v) {
        _value -= v;
        if (_value < Min) {
            _value += Range;
        }

        return *this;
    }

    template<typename V>
    wrapping_range& operator*=(V&& v) requires RootedAtZero {
        *this = _value * v;

        return *this;
    }

    template<typename V>
    wrapping_range& operator/=(V&& v) requires RootedAtZero {
        *this = _value / v;

        return *this;
    }
};

} // namespace tools

#endif//CPPTOOLS_UTILITY_WRAPPING_RANGE_HPP