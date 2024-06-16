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

std::string_view parameter_exception::to_string() const
{
    _str = std::string(base_exception::to_string()) + '\n' +
        "Parameter: " + _parameter_name + ", value: " + _parameter_value;

    return _str;
}

}