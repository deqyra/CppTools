#include "exception.hpp"

#include <cpptools/utility/to_string.hpp>

namespace tools::exception
{

const char* base_exception::what() const
{
    return to_string().data();
}

std::string_view base_exception::to_string() const
{
    static const std::string result =
        "Category: " + std::string(error_category_name(category())) +
        ", error: (" + std::to_string(error_code()) + ") " + std::string(ecode_to_string()) + '\n' +
        "Location: " + tools::to_string(source_location()) + '\n' +
        "Message: "  + std::string(message());

    return result;
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