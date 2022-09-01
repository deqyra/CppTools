#include "exception.hpp"

#include <sstream>

namespace tools::exception
{

std::string base_exception::to_string() const
{
    std::stringstream result;
    result << "Category: " << error_category_name(category())
           << ", error: (" << error_code() << ") " << error_code_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

std::ostream& operator<<(std::ostream& out, const base_exception& e)
{
    return out << e.to_string();
}

template<>
constexpr std::string_view default_error_message<unknown_exception::error_code>(const unknown_exception::error_code& code)
{
    switch (code)
    {
        case unknown_exception::error_code::unknown:
            return "An unknown error occurred";

        default:
            return "???";
    }
}

template<>
constexpr std::string_view to_string<unknown_exception::error_code>(const unknown_exception::error_code& code)
{
    switch (code)
    {
        case unknown_exception::error_code::unknown:
            return "unknown";

        default:
            return "???";
    }
}

} // namespace tools::exception