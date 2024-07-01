#ifndef CPPTOOLS_UTILITY_MONITORED_VALUE_HPP
#define CPPTOOLS_UTILITY_MONITORED_VALUE_HPP

#include <utility>
#include <functional>

namespace tools {

/// @brief Simple wrapper which raises a flag whenever the contained value is modified
template<typename T>
class monitored_value {
    T _value;
    bool& _flag;

public:
    monitored_value(T v, bool& raise_on_modification) :
        _value(std::move(v)),
        _flag(raise_on_modification)
    {
        
    }

    monitored_value(const monitored_value&) = delete;
    monitored_value(monitored_value&&) = delete;
    monitored_value& operator=(const monitored_value&) = delete;
    monitored_value& operator=(monitored_value&&) = delete;

    operator const T&() const {
        return _value;
    }

    const T& get() const {
        return _value;
    }

    operator T&&() && {
        _flag = true;
        return std::move(_value);
    }

    monitored_value& operator=(const T& v) {
        _value = v;
        _flag = true;

        return *this;
    }

    monitored_value& operator=(T&& v) {
        _value = std::move(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator+=(V&& v) {
        _value += std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator-=(V&& v) {
        _value -= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator*=(V&& v) {
        _value *= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator/=(V&& v) {
        _value /= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator%=(V&& v) {
        _value %= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator&=(V&& v) {
        _value &= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator|=(V&& v) {
        _value |= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator^=(V&& v) {
        _value ^= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator<<=(V&& v) {
        _value <<= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename V>
    monitored_value& operator>>=(V&& v) {
        _value >>= std::forward<V>(v);
        _flag = true;

        return *this;
    }

    template<typename... Args>
    decltype(auto) operator()(Args&&... args) {
        _flag = true;

        return std::invoke(_value, std::forward<Args>(args)...);
    }

    template<typename... Args>
    decltype(auto) operator()(Args&&... args) const {
        return std::invoke(_value, std::forward<Args>(args)...);
    }

    decltype(auto) operator[](std::size_t i) {
        _flag = true;

        return _value[i];
    }

    decltype(auto) operator[](std::size_t i) const {
        return _value[i];
    }
};

} // namespace tools

#endif//CPPTOOLS_UTILITY_MONITORED_VALUE_HPP