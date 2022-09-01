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
    enum class error_code
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
    using error_code = iterator_exception::error_code;
    using it_type = It;

    template_iterator_exception(It iterator);

    std::string to_string() const override;

    const It& iterator() const;

private:
    It _iterator;
};
*/

template<>
constexpr std::string_view default_error_message<iterator_exception::error_code>(const iterator_exception::error_code& code);

template<>
constexpr std::string_view to_string<iterator_exception::error_code>(const iterator_exception::error_code& code);

namespace iterator
{
    using e = iterator_exception::error_code;

    using incremented_past_end_error = exception<iterator_exception, e::incremented_past_end>;
    using decremented_past_begin_error = exception<iterator_exception, e::decremented_past_begin>;
    using illegal_dereference_error = exception<iterator_exception, e::illegal_dereference>;
    using operation_while_invalid_error = exception<iterator_exception, e::operation_while_invalid>;
}

} // namespace tools::exception

#endif//CPPTOOLS__EXCEPTION__ITERATOR_EXCEPTION_HPP