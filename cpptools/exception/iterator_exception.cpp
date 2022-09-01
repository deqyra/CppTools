#include "iterator_exception.hpp"

#include <sstream>

namespace cpptools::exception
{

template<>
constexpr std::string_view default_error_message<iterator_exception::error_code>(const iterator_exception::error_code& code)
{
    switch (code)
    {
    case iterator_exception::error_code::incremented_past_end:
        return "Iterator was incremented past end of container";
    case iterator_exception::error_code::decremented_past_begin:
        return "Iterator was decremented past begin of container";
    case iterator_exception::error_code::illegal_dereference:
        return "Invalid iterator was dereferenced";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string<iterator_exception::error_code>(const iterator_exception::error_code& code)
{
    switch (code)
    {
    case iterator_exception::error_code::incremented_past_end:
        return "incremented_past_end";
    case iterator_exception::error_code::decremented_past_begin:
        return "decremented_past_begin";
    case iterator_exception::error_code::illegal_dereference:
        return "illegal_dereference";

    default:
        return "???";
    }
}

}