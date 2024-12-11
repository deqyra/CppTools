#ifndef CPPTOOLS_EXCEPTION_LOOKUP_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_LOOKUP_EXCEPTION_HPP

#include <string>
#include <string_view>

#include <cpptools/api.hpp>
#include <cpptools/utility/to_string.hpp>

#include "exception.hpp"
#include "error_category_t.hpp"

namespace tools::exception {

class lookup_exception : public base_exception<error_category_t> {
public:
    using error_category_t = tools::exception::error_category_t;
    static constexpr error_category_t error_category = error_category_t::lookup;

    enum class error_code_t {
        index_out_of_bounds = 0,
        no_such_element     = 1
    };

    template<typename T>
        requires (!std::is_same_v<std::remove_cvref_t<T>, lookup_exception>)  // <- needed or this constructor would be too good a match for the copy- and move-constructors to be selected
    lookup_exception(T&& value) :
        _value(tools::to_string(std::forward<T>(value)))
    {

    }

    CPPTOOLS_API std::string_view to_string() const override {
        base_exception::_str = std::string(base_exception::to_string()) + '\n' +
            "Looked up value: " + _value + '\n';

        return base_exception::_str;
    }

private:
    std::string _value;
};

template<>
constexpr std::string_view default_error_message(const lookup_exception::error_code_t& code) {
    using enum lookup_exception::error_code_t;

    switch (code) {
    case index_out_of_bounds:
        return "The provided index was out of bounds";
    case no_such_element:
        return "No such item exists";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string(const lookup_exception::error_code_t& code) {
    using enum lookup_exception::error_code_t;

    switch (code) {
    case index_out_of_bounds:
        return "index_out_of_bounds";
    case no_such_element:
        return "no_such_element";

    default:
        return "???";
    }
}

static_assert(concrete_exception<lookup_exception>);

namespace lookup {
    using enum lookup_exception::error_code_t;

    using index_out_of_bounds_error = exception<lookup_exception, index_out_of_bounds>;
    using no_such_element_error     = exception<lookup_exception, no_such_element>;
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_LOOKUP_EXCEPTION_HPP