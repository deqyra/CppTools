#ifndef CPPTOOLS__EXCEPTION__EXCEPTION_CATEGORY_HPP
#define CPPTOOLS__EXCEPTION__EXCEPTION_CATEGORY_HPP

#include <string_view>

namespace tools::exception
{

enum class error_category
{
    unknown     = 0,
    internal    = 1,
    parameter   = 2,
    range       = 3,
    iterator    = 4,
    io          = 5
};

std::string_view error_category_name(error_category cat);

} // namespace tools::exception

#endif//CPPTOOLS__EXCEPTION__EXCEPTION_CATEGORY_HPP