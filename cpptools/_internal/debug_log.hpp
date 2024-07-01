#ifndef CPPTOOLS_INTERNAL_DEBUG_LOG_HPP
#define CPPTOOLS_INTERNAL_DEBUG_LOG_HPP

#include <array>
#include <iostream>
#include <string_view>

#include <cpptools/api.hpp>

namespace tools::internal::debug_log {

enum level : unsigned char {
    none        = 0,
    critical    = 1,
    pedantic    = 2,
    extra       = 3
};

inline constexpr std::array<std::string_view, 4> _level_names = {
    "none",
    "critical",
    "pedantic",
    "extra"
};

inline void log(level lv, std::string_view channel, std::string_view message) {
    std::cerr << '[' << _level_names[lv] << "][" << channel << "] " << message << std::endl;
}

} // namespace tools::internal::debug_log


#endif//CPPTOOLS_INTERNAL_DEBUG_LOG_HPP