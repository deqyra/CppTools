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

std::string_view range_exception::to_string() const
{
    std::string range =
        "{ " + std::to_string(_expected_range.first) +
        " ; " + std::to_string(_expected_range.first) +
        " }";

    _str = std::string(base_exception::to_string()) + '\n' +
        "Expected range: " + std::move(range) + ", value: " + std::to_string(_value) + '\n';

    return _str;
}

}
