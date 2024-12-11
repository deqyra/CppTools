#ifndef CPPTOOLS_EXCEPTION_ITERATOR_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_ITERATOR_EXCEPTION_HPP

#include <string_view>

#include "exception.hpp"
#include "error_category_t.hpp"

namespace tools::exception {

class iterator_exception : public base_exception<error_category_t> {
public:
    using error_category_t = tools::exception::error_category_t;
    static constexpr error_category_t error_category = error_category_t::iterator;

    enum class error_code_t {
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
    static constexpr enum error_category_t error_category_t = iterator_exception::error_category_t;
    using error_code_t = iterator_exception::error_code_t;
    using it_type = It;

    template_iterator_exception(It iterator);

    std::string to_string() const override;

    const It& iterator() const;

private:
    It _iterator;
};
*/

template<>
constexpr std::string_view default_error_message<iterator_exception::error_code_t>(const iterator_exception::error_code_t& code) {
    using enum iterator_exception::error_code_t;

    switch (code) {
    case incremented_past_end:
        return "Iterator was incremented past end of container";
    case decremented_past_begin:
        return "Iterator was decremented past begin of container";
    case illegal_dereference:
        return "Invalid iterator was dereferenced";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string<iterator_exception::error_code_t>(const iterator_exception::error_code_t& code) {
    using enum iterator_exception::error_code_t;

    switch (code) {
    case incremented_past_end:
        return "incremented_past_end";
    case decremented_past_begin:
        return "decremented_past_begin";
    case illegal_dereference:
        return "illegal_dereference";

    default:
        return "???";
    }
}

static_assert(concrete_exception<iterator_exception>);

namespace iterator {
    using enum iterator_exception::error_code_t;

    using incremented_past_end_error    = exception<iterator_exception, incremented_past_end>;
    using decremented_past_begin_error  = exception<iterator_exception, decremented_past_begin>;
    using illegal_dereference_error     = exception<iterator_exception, illegal_dereference>;
    using operation_while_invalid_error = exception<iterator_exception, operation_while_invalid>;
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_ITERATOR_EXCEPTION_HPP