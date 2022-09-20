#ifndef CPPTOOLS__EXCEPTION__RANGE_EXCEPTION_HPP
#define CPPTOOLS__EXCEPTION__RANGE_EXCEPTION_HPP

#include <string_view>

#include "exception.hpp"
#include "error_category.hpp"

namespace tools::exception
{

class range_exception : public base_exception
{
public:
    static constexpr error_category error_category = error_category::range;
    enum class ecode
    {
        index_out_of_bounds     = 0,
        range_bounds_exceeded   = 1
    };

    range_exception(std::pair<size_t, size_t> expected_range, size_t value);

    std::string to_string() const override;

private:
    std::pair<size_t, size_t> _expected_range;
    size_t _value;
};

template<>
constexpr std::string_view default_error_message(const range_exception::ecode& code)
{
    using e = range_exception::ecode;

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
constexpr std::string_view to_string(const range_exception::ecode& code)
{
    using e = range_exception::ecode;

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

namespace range
{
    using e = range_exception::ecode;

    using index_out_of_bounds_error = exception<range_exception, e::index_out_of_bounds>;
    using range_bounds_exceeded_error = exception<range_exception, e::range_bounds_exceeded>;
}

} // namespace tools::exception

#endif//CPPTOOLS__EXCEPTION__RANGE_EXCEPTION_HPP