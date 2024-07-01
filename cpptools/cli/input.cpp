#include "input.hpp"

namespace tools::detail {

template<>
std::string parse_as(std::string_view input) {
    return std::string(input);
}

template<>
int parse_as(std::string_view input) {
    if (!string::is_integer(input)) {
        throw std::invalid_argument("parse_string<int>: String to parse is not exclusively made of digits and a minus sign, or it is at a wrong position.");
    }
    return std::stoi(std::string(input));
}

template<>
bool parse_as(std::string_view input) {
    if (input == "y" || input == "yes" || input == "true") {
        return true;
    }

    if (input == "n" || input == "no" || input == "false") {
        return false;
    }

    throw std::invalid_argument("parse_string<bool>: Invalid string value for expected bool input.");
}

template <>
std::string_view type_name<std::string>() {
    return "string";
}

template <>
std::string_view type_name<int>() {
    return "integer";
}

template <>
std::string_view type_name<bool>() {
    return "boolean (\"y\", \"yes\", \"true\", \"n\", \"no\", \"false\")";
}

} // namespace tools::detail
