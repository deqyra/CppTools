#include "to_string.hpp"

namespace tools::detail::to_string
{

std::string to_string(const std::source_location& location)
{
    std::ostringstream ss;
    ss  << location.file_name() << "("
        << location.line() << ":"
        << location.column() << ") `"
        << location.function_name() << "`";

    return ss.str();
}

}