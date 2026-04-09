#ifndef CPPTOOLS_UTILITY_WIDEN_NARROW_HPP
#define CPPTOOLS_UTILITY_WIDEN_NARROW_HPP

#include <string>
#include <string_view>

#include <cpptools/api.hpp>

namespace tools {

/// @brief Convert a wide string into a narrow string
/// @param wstr The wide string to convert
/// @return A narrow string
CPPTOOLS_API std::string narrow(std::wstring_view wstr);

/// @brief Convert a narrow string into a wide string
/// @param wstr The narrow string to convert
/// @return A wide string
CPPTOOLS_API std::wstring widen(std::string_view str);

} // namespace tools

#endif//CPPTOOLS_UTILITY_WIDEN_NARROW_HPP