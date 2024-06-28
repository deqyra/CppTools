#ifndef CPPTOOLS_INTERNAL_DEBUG_LOG_HPP
#define CPPTOOLS_INTERNAL_DEBUG_LOG_HPP

#include <map>
#include <string_view>

namespace tools::internal::debug_log {

enum level : unsigned char {
    none        = 0,
    critical    = 1,
    pedantic    = 2,
    extra       = 3
};

extern std::map<std::string_view, level> channel_levels;

void log(level lv, std::string_view channel, std::string_view message);

} // namespace tools::internal::debug_log


#endif//CPPTOOLS_INTERNAL_DEBUG_LOG_HPP