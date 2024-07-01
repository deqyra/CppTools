#ifndef CPPTOOLS_UTILITY_CLAMPED_RANGE_HPP
#define CPPTOOLS_UTILITY_CLAMPED_RANGE_HPP

#include <algorithm>

#include <cpptools/utility/concepts.hpp>

namespace tools {

/// @brief Wrapper which prevents its contained value from exceeding whichever
/// bound of the range it represents
template<arithmetic T, T Min, T Max>
class clamped_range {
    static_assert(Min <  Max, "Min cannot be greater than Max");
    static_assert(Min != Max, "Min and Max cannot be equal");

    static inline constexpr T Zero = static_cast<T>(0);
    static inline constexpr T Range = Max - Min;
    static inline constexpr bool RootedAtZero = (Max == Zero || Min == Zero);

    T _value;

public:
    clamped_range(T v) :
        _value(v)
    {
        
    }

    clamped_range(const clamped_range&) = default;
    clamped_range(clamped_range&&) = default;
    clamped_range& operator=(const clamped_range&) = default;
    clamped_range& operator=(clamped_range&&) = default;

    operator const T&() const {
        return _value;
    }

    template<typename V>
    clamped_range& operator=(V v) {
        _value = std::clamp(v, Min, Max);

        return *this;
    }

    template<typename V>
    clamped_range& operator+=(V v) {
        _value = std::clamp(_value + v, Min, Max);

        return *this;
    }

    template<typename V>
    clamped_range& operator-=(V v) {
        _value = std::clamp(_value - v, Min, Max);

        return *this;
    }

    template<typename V>
    clamped_range& operator*=(V&& v) requires RootedAtZero {
        _value = std::clamp(_value * v, Min, Max);

        return *this;
    }

    template<typename V>
    clamped_range& operator/=(V&& v) requires RootedAtZero {
        _value = std::clamp(_value / v, Min, Max);

        return *this;
    }
};

} // namespace tools

#endif//CPPTOOLS_UTILITY_CLAMPED_RANGE_HPP