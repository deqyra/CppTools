#include "parameter_exception.hpp"

#include <sstream>

namespace tools::exception
{

parameter_exception::parameter_exception(std::string_view parameter_name, std::string parameter_value) :
    base_exception(),
    _parameter_name(parameter_name),
    _parameter_value(parameter_value)
{
}

std::string_view parameter_exception::parameter_name() const
{
    return _parameter_name;
}

std::string_view parameter_exception::parameter_value() const
{
    return _parameter_value;
}

std::string parameter_exception::to_string() const
{
    std::stringstream result;
    result << "Category: " << error_category_name(category())
           << ", error: (" << error_code() << ") " << ecode_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Parameter: " << _parameter_name << ", value: " << _parameter_value << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

}