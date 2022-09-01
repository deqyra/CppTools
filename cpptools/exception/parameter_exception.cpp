#include "parameter_exception.hpp"

#include <sstream>

namespace cpptools::exception
{

parameter_exception::parameter_exception(const std::string_view& parameter_name, std::string parameter_value) :
    base_exception(),
    _parameter_name(parameter_name),
    _parameter_value(parameter_value)
{
}

const std::string& parameter_exception::parameter_name() const
{
    return _parameter_name;
}

const std::string& parameter_exception::parameter_value() const
{
    return _parameter_value;
}

std::string parameter_exception::to_string() const
{
    std::stringstream result;
    result << "Category: " << error_category_name(category())
           << ", error: (" << base_exception::error_code() << ") " << error_code_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Parameter: " << _parameter_name << ", value: " << _parameter_value << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

template<>
constexpr std::string_view default_error_message<parameter_exception::error_code>(const parameter_exception::error_code& code)
{
    switch (code)
    {
    case parameter_exception::error_code::invalid_value:
        return "Parameter has invalid value";
    case parameter_exception::error_code::null_parameter:
        return "Parameter is null";
    case parameter_exception::error_code::lossy_cast:
        return "A data loss would incur from casting to the expected type";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string<parameter_exception::error_code>(const parameter_exception::error_code& code)
{
    switch (code)
    {
    case parameter_exception::error_code::invalid_value:
        return "invalid_value";
    case parameter_exception::error_code::null_parameter:
        return "null_parameter";
    case parameter_exception::error_code::lossy_cast:
        return "lossy_cast";

    default:
        return "???";
    }
}

}