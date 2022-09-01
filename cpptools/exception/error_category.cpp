#include "error_category.hpp"

namespace cpptools::exception
{

std::string_view to_string(error_category cat)
{
    switch (cat)
    {
    case error_category::unknown:   return "unknown";
    case error_category::internal:  return "internal";
    case error_category::parameter: return "parameter";
    case error_category::range:     return "range";
    case error_category::iterator:  return "iterator";
    case error_category::io:        return "io";

    default:                        return "???";
    }
}

} // namespace cpptools::exception
