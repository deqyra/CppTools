#ifndef CPPTOOLS_EXCEPTION_INTERNAL_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_INTERNAL_EXCEPTION_HPP

#include <string_view>

#include "exception.hpp"
#include "error_category_t.hpp"

namespace tools::exception {

class internal_exception : public base_exception<error_category_t> {
public:
    using error_category_t = tools::exception::error_category_t;
    static constexpr error_category_t error_category = error_category_t::internal;

    enum class error_code_t {
        out_of_memory           = 0,
        not_implemented         = 1,
        invalid_state           = 2,
        precondition_failure    = 3,
        postcondition_failure   = 4,
        unexpected              = 5
    };
};

template<>
constexpr std::string_view default_error_message(const internal_exception::error_code_t& code) {
    using enum internal_exception::error_code_t;

    switch (code) {
    case out_of_memory:
        return "Out of memory";
    case not_implemented:
        return "Not implemented";
    case invalid_state:
        return "Invalid state";
    case precondition_failure:
        return "Precondition failure";
    case postcondition_failure:
        return "Postcondition failure";
    case unexpected:
        return "An unexpected error occurred";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string(const internal_exception::error_code_t& code) {
    using enum internal_exception::error_code_t;

    switch (code) {
    case out_of_memory:
        return "out_of_memory";
    case not_implemented:
        return "not_implemented";
    case invalid_state:
        return "invalid_state";
    case precondition_failure:
        return "precondition_failure";
    case postcondition_failure:
        return "postcondition_failure";
    case unexpected:
        return "unexpected";

    default:
        return "???";
    }
}

static_assert(concrete_exception<internal_exception>);

namespace internal {
    using enum internal_exception::error_code_t;

    using out_of_memory_error         = exception<internal_exception, out_of_memory>;
    using not_implemented_error       = exception<internal_exception, not_implemented>;
    using invalid_state_error         = exception<internal_exception, invalid_state>;
    using precondition_failure_error  = exception<internal_exception, precondition_failure>;
    using postcondition_failure_error = exception<internal_exception, postcondition_failure>;
    using unexpected_error            = exception<internal_exception, unexpected>;
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_INTERNAL_EXCEPTION_HPP
