#ifndef CPPTOOLS__EXCEPTION__INTERNAL_EXCEPTION_HPP
#define CPPTOOLS__EXCEPTION__INTERNAL_EXCEPTION_HPP

#include <string_view>

#include "exception.hpp"
#include "error_category.hpp"

namespace cpptools::exception
{

class internal_exception : public base_exception
{
public:
    static constexpr error_category error_category = error_category::internal;
    enum class error_code
    {
        out_of_memory           = 0,
        not_implemented         = 1,
        invalid_state           = 2,
        precondition_failure    = 3,
        postcondition_failure   = 4,
        unexpected              = 5
    };
};

template<>
constexpr std::string_view default_error_message(const internal_exception::error_code& code);

template<>
constexpr std::string_view to_string(const internal_exception::error_code& code);

namespace internal
{
    using e = internal_exception::error_code;

    using out_of_memory_error = exception<internal_exception, e::out_of_memory>;
    using not_implemented_error = exception<internal_exception, e::not_implemented>;
    using invalid_state_error = exception<internal_exception, e::invalid_state>;
    using precondition_failure_error = exception<internal_exception, e::precondition_failure>;
    using postcondition_failure_error = exception<internal_exception, e::postcondition_failure>;
    using unexpected_error = exception<internal_exception, e::unexpected>;
}

} // namespace cpptools::exception

#endif//CPPTOOLS__EXCEPTION__INTERNAL_EXCEPTION_HPP
