#include "cli_input.hpp"

namespace tools::cli_input
{

template<>
std::string parse_string(const std::string& input)
{
    return input;
}

template<>
int parse_string(const std::string& input)
{
    if (!string::is_integer(input))
    {
        throw std::invalid_argument("parse_string<int>: String to parse is not exclusively made of digits and a minus sign, or it is at a wrong position.");
    }
    return std::stoi(input);
}

template<>
bool parse_string(const std::string& input)
{
    if (input == "y" || input == "yes" || input == "true")
    {
        return true;
    }

    if (input == "n" || input == "no" || input == "false")
    {
        return false;
    }

    throw std::invalid_argument("parse_string<bool>: Invalid string value for expected bool input.");
}

//
// Specializations of type_name.
//

template <>
std::string type_name<std::string>()
{
    return "string";
}

template <>
std::string type_name<int>()
{
    return "integer";
}

template <>
std::string type_name<bool>()
{
    return "boolean (\"y\", \"yes\", \"true\", \"n\", \"no\", \"false\")";
}

} // namespace tools::cli_input
