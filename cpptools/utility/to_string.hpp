#ifndef CPPTOOLS_UTILITY_TO_STRING_HPP
#define CPPTOOLS_UTILITY_TO_STRING_HPP

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <source_location>
#include <sstream>
#include <string>

#include <cpptools/api.hpp>
#include <cpptools/utility/string.hpp>

namespace tools::detail::to_string { // additional namespace because there's a using directive in there, and namespace detail should not be polluted by it

CPPTOOLS_API inline std::string to_string(const std::source_location& location) {
    std::ostringstream ss;
    ss  << location.file_name() << "("
        << location.line() << ":"
        << location.column() << ") `"
        << location.function_name() << "`";

    return std::move(ss).str();
}

CPPTOOLS_API inline std::string to_string(std::nullptr_t) {
    return "nullptr";
}

template<typename T>
std::string to_string(T* ptr) {
    if (ptr == nullptr) {
        return "nullptr";
    }

    std::ostringstream ss;
    ss << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(std::intptr_t) * 2) << reinterpret_cast<std::intptr_t>(ptr);

    return ss.str();
}

CPPTOOLS_API inline std::string to_string(char c) {
    return std::string{} + c;
}

CPPTOOLS_API inline std::string to_string(std::string_view v) {
    return std::string(v);
}

CPPTOOLS_API inline std::string to_string(const std::string& s) {
    return s;
}

CPPTOOLS_API inline std::string to_string(wchar_t c) {
    return narrow(std::wstring{} + c);
}

CPPTOOLS_API inline std::string to_string(std::wstring_view v) {
    return narrow(v);
}

CPPTOOLS_API inline std::string to_string(const std::wstring& s) {
    return narrow(s);
}

using std::to_string;

struct to_string_cpo {
    template<typename T>
    std::string operator()(T&& t) const {
        return to_string(std::forward<T>(t));
    }
};

} // namespace tools::detail

namespace tools {

inline constexpr auto to_string = detail::to_string::to_string_cpo{};

/// @brief Satisfied by any type on which the \c to_string CPO can be called.
/// The \c to_string CPO tries to call a \c to_string function with the given
/// type. While some useful overloads are provided, desired overloads should be
/// visible (possibly through ADL) at the point of use of this concept.
template<typename T>
concept stringable = requires (T v) {
    to_string(v);
};

} // namespace tools

#endif//CPPTOOLS_UTILITY_TO_STRING_HPP