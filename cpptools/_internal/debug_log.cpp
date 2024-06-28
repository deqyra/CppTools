#include "debug_log.hpp"

#include <iostream>
#include <ostream>

namespace tools::internal::debug_log {

namespace {
    std::ostream& _out = std::cerr;
    const char* _level_names[] = {
        "none",
        "critical",
        "pedantic",
        "extra"
    };
}

std::map<std::string_view, level> channel_levels = {};

void log(level lv, std::string_view channel, std::string_view message) {
    auto it = channel_levels.find(channel);
    if (it != channel_levels.end() && (lv <= it->second))
    {
        _out << '[' << _level_names[lv] << "][" << channel << "] " << message << std::endl;
    }
}

} // namespace tools::internal::debug_log