#ifndef CPPTOOLS_EXCEPTION_ERROR_CATEGORY_T_HPP
#define CPPTOOLS_EXCEPTION_ERROR_CATEGORY_T_HPP

#include <string_view>

#include <cpptools/api.hpp>

namespace tools::exception {

/// @brief cpptools error categories
enum class error_category_t {
    unknown     = 0,
    internal    = 1,
    parameter   = 2,
    lookup      = 3,
    iterator    = 4,
    io          = 5,
    arg_parse   = 6
};

CPPTOOLS_API inline std::string_view to_string(error_category_t cat) noexcept {
    switch (cat) {
    case error_category_t::unknown:   return "unknown";
    case error_category_t::internal:  return "internal";
    case error_category_t::parameter: return "parameter";
    case error_category_t::lookup:    return "lookup";
    case error_category_t::iterator:  return "iterator";
    case error_category_t::io:        return "io";
    case error_category_t::arg_parse: return "arg_parse";

    default:                   return "???";
    }
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_ERROR_CATEGORY_T_HPP