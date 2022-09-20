#ifndef CPPTOOLS__EXCEPTION__PARAMETER_EXCEPTION_HPP
#define CPPTOOLS__EXCEPTION__PARAMETER_EXCEPTION_HPP

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "exception.hpp"
#include "error_category.hpp"

namespace tools::exception
{

class parameter_exception : public base_exception
{
public:
    static constexpr enum error_category error_category = error_category::parameter;
    enum class ecode
    {
        invalid_value   = 0,
        null_parameter  = 1,
        lossy_cast      = 2
    };

    parameter_exception(std::string_view parameter_name, std::string parameter_value = "<undefined>");

    template<typename T>
    parameter_exception(std::string_view parameter_name, const T& parameter_value) :
        base_exception(),
        _parameter_name(parameter_name),
        _parameter_value()
    {
        using std::to_string;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-template"

        _parameter_value = to_string(parameter_value);

#pragma clang diagnostic pop
    }

    std::string_view parameter_name() const;
    std::string_view parameter_value() const;

    std::string to_string() const override;

private:
    std::string _parameter_name;
    std::string _parameter_value;
};

template<>
constexpr std::string_view default_error_message<parameter_exception::ecode>(const parameter_exception::ecode& code)
{
    switch (code)
    {
    case parameter_exception::ecode::invalid_value:
        return "Parameter has invalid value";
    case parameter_exception::ecode::null_parameter:
        return "Parameter is null";
    case parameter_exception::ecode::lossy_cast:
        return "A data loss would incur from casting to the expected type";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string<parameter_exception::ecode>(const parameter_exception::ecode& code)
{
    switch (code)
    {
    case parameter_exception::ecode::invalid_value:
        return "invalid_value";
    case parameter_exception::ecode::null_parameter:
        return "null_parameter";
    case parameter_exception::ecode::lossy_cast:
        return "lossy_cast";

    default:
        return "???";
    }
}

namespace parameter
{
    using e = parameter_exception::ecode;

    using invalid_value_error = exception<parameter_exception, e::invalid_value>;
    using null_parameter_error = exception<parameter_exception, e::null_parameter>;
    using lossy_cast_error = exception<parameter_exception, e::lossy_cast>;
}

} // namespace tools::exception

#endif//CPPTOOLS__EXCEPTION__PARAMETER_EXCEPTION_HPP