#ifndef CPPTOOLS_EXCEPTION_ARG_PARSE_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_ARG_PARSE_EXCEPTION_HPP

#include <string_view>

#include <cpptools/api.hpp>
#include <cpptools/exception/exception.hpp>
#include <cpptools/utility/string.hpp>

namespace tools::exception {

class arg_parse_exception : public base_exception {
public:
    static constexpr enum error_category error_category = error_category::arg_parse;
    enum class ecode {
        multiple_params_consume_remaining_args = 0,
        multiple_params_with_same_name         = 1,
        param_with_no_name                     = 2,
        not_enough_args_supplied               = 3,
        required_arg_missing                   = 4
    };

    CPPTOOLS_API arg_parse_exception() = default;

    CPPTOOLS_API arg_parse_exception(std::string_view arg_name) :
        _arg_name(arg_name)
    {

    }

    CPPTOOLS_API arg_parse_exception(std::wstring_view arg_name) :
        _arg_name(narrow(arg_name))
    {

    }

    CPPTOOLS_API std::string_view to_string() const override {
        if (_arg_name.size() == 0) {
            return base_exception::to_string();
        }
        
        base_exception::_str = std::string(base_exception::to_string()) + '\n' + "Argument name: " + _arg_name;

        return base_exception::_str;
    }

    CPPTOOLS_API std::string_view arg_name() const {
        return _arg_name;
    }

private:
    std::string _arg_name;
};

template<>
constexpr std::string_view default_error_message<arg_parse_exception::ecode>(const arg_parse_exception::ecode& code) {
    switch (code) {
    case arg_parse_exception::ecode::multiple_params_consume_remaining_args:
        return "Multiple parameters were specified to consume all remaining arguments";
    case arg_parse_exception::ecode::multiple_params_with_same_name:
        return "Multiple parameters were specified with the same name";
    case arg_parse_exception::ecode::param_with_no_name:
        return "A parameter was specified with no name";
    case arg_parse_exception::ecode::not_enough_args_supplied:
        return "Not enough arguments were supplied to satisfy a parameter";
    case arg_parse_exception::ecode::required_arg_missing:
        return "No argument was found for a parameter specified as required";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string<arg_parse_exception::ecode>(const arg_parse_exception::ecode& code) {
    switch (code) {
    case arg_parse_exception::ecode::multiple_params_consume_remaining_args:
        return "multiple_consume_remaining_args";
    case arg_parse_exception::ecode::multiple_params_with_same_name:
        return "multiple_params_with_same_name";
    case arg_parse_exception::ecode::param_with_no_name:
        return "param_with_no_name";
    case arg_parse_exception::ecode::not_enough_args_supplied:
        return "not_enough_arguments_supplied";
    case arg_parse_exception::ecode::required_arg_missing:
        return "required_arg_missing";

    default:
        return "???";
    }
}

namespace arg_parse {
    using e = arg_parse_exception::ecode;

    using multiple_consume_remaining_args_error = exception<arg_parse_exception, e::multiple_params_consume_remaining_args>;
    using multiple_params_with_same_name_error  = exception<arg_parse_exception, e::multiple_params_with_same_name>;
    using param_with_no_name_error              = exception<arg_parse_exception, e::param_with_no_name>;
    using not_enough_arguments_supplied_error   = exception<arg_parse_exception, e::not_enough_args_supplied>;
    using required_arg_missing_error            = exception<arg_parse_exception, e::required_arg_missing>;
}


} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_ARG_PARSE_EXCEPTION_HPP