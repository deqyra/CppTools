#include "exception.hpp"

#include <sstream>

namespace tools::exception
{

std::string base_exception::to_string() const
{
    std::stringstream result;
    result << "Category: " << error_category_name(category())
           << ", error: (" << error_code() << ") " << ecode_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

std::ostream& operator<<(std::ostream& out, const base_exception& e)
{
    return out << e.to_string();
}

template<>
constexpr std::string_view default_error_message<unknown_exception::ecode>(const unknown_exception::ecode& code)
{
    switch (code)
    {
        case unknown_exception::ecode::unknown:
            return "An unknown error occurred";

        default:
            return "???";
    }
}

template<>
constexpr std::string_view to_string<unknown_exception::ecode>(const unknown_exception::ecode& code)
{
    switch (code)
    {
        case unknown_exception::ecode::unknown:
            return "unknown";

        default:
            return "???";
    }
}

} // namespace tools::exception