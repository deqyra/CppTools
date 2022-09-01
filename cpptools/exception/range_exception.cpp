#include "range_exception.hpp"
#include "exception.hpp"

#include <sstream>

namespace tools::exception
{

range_exception::range_exception(std::pair<size_t, size_t> expected_range, size_t value) :
    base_exception(),
    _expected_range(std::move(expected_range)),
    _value(value)
{

}

std::string range_exception::to_string() const
{
    std::stringstream result;
    result << "Category: " << error_category_name(category())
           << ", error: (" << base_exception::error_code() << ") " << error_code_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Expected range: { " << _expected_range.first << " ; " << _expected_range.first << " }, value: " << _value << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

template<>
constexpr std::string_view default_error_message(const range_exception::error_code& code)
{
    using e = range_exception::error_code;

    switch (code)
    {
    case e::index_out_of_bounds:
        return "The provided index was out of bounds";
    case e::range_bounds_exceeded:
        return "The provided value was outside the expected range";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string(const range_exception::error_code& code)
{
    using e = range_exception::error_code;

    switch (code)
    {
    case e::index_out_of_bounds:
        return "index_out_of_bounds";
    case e::range_bounds_exceeded:
        return "range_bounds_exceeded";

    default:
        return "???";
    }
}

}
