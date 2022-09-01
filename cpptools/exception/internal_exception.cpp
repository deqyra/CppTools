#include "internal_exception.hpp"

namespace cpptools::exception
{

template<>
constexpr std::string_view default_error_message(const internal_exception::error_code& code)
{
    using e = internal_exception::error_code;

    switch (code)
    {
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
constexpr std::string_view to_string(const internal_exception::error_code& code)
{
    using e = internal_exception::error_code;

    switch (code)
    {
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

}
