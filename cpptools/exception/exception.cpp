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
    _str =
        "Category: " + std::string(error_category_name(category())) +
        ", error: (" + std::to_string(error_code()) + ") " + std::string(ecode_to_string()) + '\n' +
        "Location: " + tools::to_string(source_location()) + '\n' +
        "Message: "  + std::string(message());

    return _str;
}

std::ostream& operator<<(std::ostream& out, const base_exception& e)
{
    return out << e.to_string();
}

} // namespace tools::exception