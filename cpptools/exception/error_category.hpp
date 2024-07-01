#ifndef CPPTOOLS_EXCEPTION_EXCEPTION_CATEGORY_HPP
#define CPPTOOLS_EXCEPTION_EXCEPTION_CATEGORY_HPP

#include <string_view>

#include <cpptools/api.hpp>

namespace tools::exception {

enum class error_category {
    unknown     = 0,
    internal    = 1,
    parameter   = 2,
    range       = 3,
    iterator    = 4,
    io          = 5
};

CPPTOOLS_API std::string_view error_category_name(error_category cat);

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_EXCEPTION_CATEGORY_HPP