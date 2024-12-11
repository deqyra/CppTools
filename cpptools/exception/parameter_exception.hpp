#ifndef CPPTOOLS_EXCEPTION_PARAMETER_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_PARAMETER_EXCEPTION_HPP

#include <string>
#include <string_view>

#include <cpptools/utility/to_string.hpp>
#include <cpptools/utility/concepts.hpp>

#include <cpptools/api.hpp>

#include "exception.hpp"
#include "error_category_t.hpp"

namespace tools::exception {

class parameter_exception : public base_exception<error_category_t> {
public:
    using error_category_t = tools::exception::error_category_t;
    static constexpr error_category_t error_category = error_category_t::parameter;

    enum class error_code_t {
        invalid_value   = 0,
        null_parameter  = 1
    };

    CPPTOOLS_API parameter_exception(std::string_view parameter_name, std::string parameter_value = "<undefined>") :
        base_exception(),
        _parameter_name(parameter_name),
        _parameter_value(std::move(parameter_value))
    {
    }

    template<stringable T>
    parameter_exception(std::string_view parameter_name, const T& parameter_value) :
        base_exception(),
        _parameter_name(parameter_name),
        _parameter_value()
    {
        _parameter_value = tools::to_string(parameter_value);
    }

    template<typename T>
    parameter_exception(std::string_view parameter_name, const T& parameter_value) :
        base_exception(),
        _parameter_name(parameter_name),
        _parameter_value()
    {
        _parameter_value = "<" + tools::to_string(&parameter_value) + ">";
    }

    CPPTOOLS_API std::string_view parameter_name()  const { return _parameter_name; }
    CPPTOOLS_API std::string_view parameter_value() const { return _parameter_value; }

    CPPTOOLS_API std::string_view to_string() const override {
        base_exception::_str = std::string(base_exception::to_string()) + '\n' +
            "Parameter: " + _parameter_name + ", value: " + _parameter_value;

        return base_exception::_str;
    }

private:
    std::string _parameter_name;
    std::string _parameter_value;
};

template<>
constexpr std::string_view default_error_message<parameter_exception::error_code_t>(const parameter_exception::error_code_t& code) {
    using enum parameter_exception::error_code_t;

    switch (code) {
    case invalid_value:
        return "Parameter has invalid value";
    case null_parameter:
        return "Parameter is null";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string<parameter_exception::error_code_t>(const parameter_exception::error_code_t& code) {
    using enum parameter_exception::error_code_t;

    switch (code) {
    case invalid_value:
        return "invalid_value";
    case null_parameter:
        return "null_parameter";

    default:
        return "???";
    }
}

static_assert(concrete_exception<parameter_exception>);

namespace parameter {
    using enum parameter_exception::error_code_t;

    using invalid_value_error  = exception<parameter_exception, invalid_value>;
    using null_parameter_error = exception<parameter_exception, null_parameter>;
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_PARAMETER_EXCEPTION_HPP