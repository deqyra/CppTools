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
           << ", error: (" << base_exception::error_code() << ") " << ecode_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Expected range: { " << _expected_range.first << " ; " << _expected_range.first << " }, value: " << _value << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

}
