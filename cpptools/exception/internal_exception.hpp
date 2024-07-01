#ifndef CPPTOOLS_EXCEPTION_INTERNAL_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_INTERNAL_EXCEPTION_HPP

#include <string_view>

#include "exception.hpp"
#include "error_category.hpp"

namespace tools::exception {

class internal_exception : public base_exception {
public:
    static constexpr error_category error_category = error_category::internal;
    enum class ecode     {
        out_of_memory           = 0,
        not_implemented         = 1,
        invalid_state           = 2,
        precondition_failure    = 3,
        postcondition_failure   = 4,
        unexpected              = 5
    };
};

template<>
constexpr std::string_view default_error_message(const internal_exception::ecode& code) {
    using e = internal_exception::ecode;

    switch (code) {
    case e::out_of_memory:
        return "Out of memory";
    case e::not_implemented:
        return "Not implemented";
    case e::invalid_state:
        return "Invalid state";
    case e::precondition_failure:
        return "Precondition failure";
    case e::postcondition_failure:
        return "Postcondition failure";
    case e::unexpected:
        return "An unexpected error occurred";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string(const internal_exception::ecode& code) {
    using e = internal_exception::ecode;

    switch (code) {
    case e::out_of_memory:
        return "out_of_memory";
    case e::not_implemented:
        return "not_implemented";
    case e::invalid_state:
        return "invalid_state";
    case e::precondition_failure:
        return "precondition_failure";
    case e::postcondition_failure:
        return "postcondition_failure";
    case e::unexpected:
        return "unexpected";

    default:
        return "???";
    }
}

namespace internal {
    using e = internal_exception::ecode;

    using out_of_memory_error         = exception<internal_exception, e::out_of_memory>;
    using not_implemented_error       = exception<internal_exception, e::not_implemented>;
    using invalid_state_error         = exception<internal_exception, e::invalid_state>;
    using precondition_failure_error  = exception<internal_exception, e::precondition_failure>;
    using postcondition_failure_error = exception<internal_exception, e::postcondition_failure>;
    using unexpected_error            = exception<internal_exception, e::unexpected>;
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_INTERNAL_EXCEPTION_HPP
