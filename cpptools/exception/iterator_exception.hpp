#ifndef CPPTOOLS__EXCEPTION__ITERATOR_EXCEPTION_HPP
#define CPPTOOLS__EXCEPTION__ITERATOR_EXCEPTION_HPP

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "exception.hpp"
#include "error_category.hpp"

namespace tools::exception
{

class iterator_exception : public base_exception
{
public:
    static constexpr enum error_category error_category = error_category::iterator;
    enum class ecode
    {
        incremented_past_end    = 0,
        decremented_past_begin  = 1,
        illegal_dereference     = 2,
        operation_while_invalid = 3
    };
};

/*
template<typename It>
class template_iterator_exception : public base_exception
{
public:
    static constexpr enum error_category error_category = iterator_exception::error_category;
    using ecode = iterator_exception::ecode;
    using it_type = It;

    template_iterator_exception(It iterator);

    std::string to_string() const override;

    const It& iterator() const;

private:
    It _iterator;
};
*/

template<>
constexpr std::string_view default_error_message<iterator_exception::ecode>(const iterator_exception::ecode& code)
{
    switch (code)
    {
    case iterator_exception::ecode::incremented_past_end:
        return "Iterator was incremented past end of container";
    case iterator_exception::ecode::decremented_past_begin:
        return "Iterator was decremented past begin of container";
    case iterator_exception::ecode::illegal_dereference:
        return "Invalid iterator was dereferenced";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string<iterator_exception::ecode>(const iterator_exception::ecode& code)
{
    switch (code)
    {
    case iterator_exception::ecode::incremented_past_end:
        return "incremented_past_end";
    case iterator_exception::ecode::decremented_past_begin:
        return "decremented_past_begin";
    case iterator_exception::ecode::illegal_dereference:
        return "illegal_dereference";

    default:
        return "???";
    }
}

namespace iterator
{
    using e = iterator_exception::ecode;

    using incremented_past_end_error = exception<iterator_exception, e::incremented_past_end>;
    using decremented_past_begin_error = exception<iterator_exception, e::decremented_past_begin>;
    using illegal_dereference_error = exception<iterator_exception, e::illegal_dereference>;
    using operation_while_invalid_error = exception<iterator_exception, e::operation_while_invalid>;
}

} // namespace tools::exception

#endif//CPPTOOLS__EXCEPTION__ITERATOR_EXCEPTION_HPP