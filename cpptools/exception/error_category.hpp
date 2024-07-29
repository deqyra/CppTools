#ifndef CPPTOOLS_EXCEPTION_EXCEPTION_CATEGORY_HPP
#define CPPTOOLS_EXCEPTION_EXCEPTION_CATEGORY_HPP

#include <string_view>

#include <cpptools/api.hpp>

namespace tools::exception {

enum class error_category {
    unknown     = 0,
    internal    = 1,
    parameter   = 2,
    lookup      = 3,
    iterator    = 4,
    io          = 5,
    arg_parse   = 6
};

CPPTOOLS_API inline std::string_view error_category_name(error_category cat) {
    switch (cat) {
    case error_category::unknown:   return "unknown";
    case error_category::internal:  return "internal";
    case error_category::parameter: return "parameter";
    case error_category::lookup:    return "lookup";
    case error_category::iterator:  return "iterator";
    case error_category::io:        return "io";
    case error_category::arg_parse: return "arg_parse";

    default:                        return "???";
    }
}


} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_EXCEPTION_CATEGORY_HPP