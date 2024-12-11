#ifndef CPPTOOLS_EXCEPTION_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_EXCEPTION_HPP

#include <ostream>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <cpptools/api.hpp>
#include <cpptools/utility/to_string.hpp>

#include "error_category_t.hpp"

namespace tools::exception
{

/// Exception model in cpptools:
///
/// - In the following, T is a "concrete" exception type but is not meant to be throwable in and of itself
/// - Instead, T represents an *error category*, a category of throwable exception classes
/// - The possible error categories are grouped as literals in enum type error_category_t
/// - Any T must satisfy the concrete_exception concept, which has the following characteristics:
///     - T inherits from base_exception
///     - T provides a public, static constexpr member named "error_category_t", of enum type error_category_t
///     - The value of T::error_category_t is a literal that represents the error category which T is meant for
///     - T provides a public enum type named "error_code_t"
///     - The literals in T::error_code_t describe error cases in the category of errors that T represents
///     - A specialization of tools::exception::default_error_message for T::error_code_t is visible alongside T
///     - A specialization of tools::exception::to_string for T::error_code_t is visible alongside T
/// - Every exception class meant to be throwable must be a specialization of the `exception` template, which:
///     - uses CRTP to inherit from a type T as described above
///     - is also template-parameterized on a specific error code literal in T::error_code

template<typename E>
consteval std::string_view default_error_message(const E& code) = delete;

template<typename E>
consteval std::string_view to_string(const E& code) = delete;

/// @brief Base class which has to be inherited by all new exception classes
/// @tparam Category Enum whose literals represent different error categories
template<stringable_enum Category>
class base_exception : public std::exception {
public:
    base_exception() = default;
    virtual ~base_exception() = default;

    using error_category_t = Category;

    virtual error_category_t category()       const noexcept = 0;
    virtual std::size_t      code()           const noexcept = 0;
    virtual std::string_view code_to_string() const noexcept = 0;

    virtual const std::source_location& source_location() const noexcept = 0;
    virtual       std::string_view      message()         const noexcept = 0;
    virtual       std::string&          message()               noexcept = 0;

    CPPTOOLS_API virtual const char* what() const noexcept override {
        return to_string().data();

        // Yes, should the call to to_string throw, std::terminate will be called
        // Not ideal but if this ever happens then there are worse concerns to be had
    }

    CPPTOOLS_API virtual std::string_view to_string() const {
        using namespace ::tools::ostream_dump;

        std::ostringstream ss;
        ss  << "Category: " << category() << ", error: (" << code() << ") " << code_to_string() << '\n'
            << "Location: " << source_location() << '\n'
            << "Message: "  << message();

        _str = std::move(ss).str();
        return _str;
    }

protected:
    mutable std::string _str;
};

template<typename ErrCat>
CPPTOOLS_API inline std::ostream& operator<<(std::ostream& out, const base_exception<ErrCat>& e) {
    return out << e.to_string();
}

template<typename T>
concept concrete_exception = requires {
    std::is_enum_v<typename T::error_code_t>;
    std::is_enum_v<typename T::error_category_t>;
    std::is_base_of_v<base_exception<typename T::error_category_t>, T>;
    std::is_same_v<decltype(T::error_category), typename T::error_category_t>;

    { default_error_message<typename T::error_code_t>(static_cast<typename T::error_code_t>(0)) } -> std::same_as<std::string_view>;
    {             to_string<typename T::error_code_t>(static_cast<typename T::error_code_t>(0)) } -> std::same_as<std::string_view>;
};

/// @brief Specialize to instantiate concrete exception classes.
///
/// @tparam T Parent exception class. Must satisfy concrete_exception.
/// @tparam Code Error code related to T's handled error category.
template<concrete_exception T, typename T::error_code_t Code>
class exception : public T {
private:
    std::source_location _source_location;
    std::string _message;

public:
    /// @param function Name of the function in which the exception was thrown
    /// @param line Number of the line where the exception was thrown
    /// @param args Arguments to forward to the constructor of T
    template<typename... ArgTypes>
    explicit exception(std::source_location&& source_location, ArgTypes&&... args) :
        T(std::forward<ArgTypes&&>(args)...),
        _source_location(std::move(source_location)),
        _message(default_error_message(Code))
    {
    }

    exception(const exception &e) = default;

    // Overrides from base_exception

    error_category_t  category()       const noexcept override { return T::error_category; }
    std::size_t       code()           const noexcept override { return static_cast<std::size_t>(Code); }
    std::string_view  code_to_string() const noexcept override { return to_string(Code); }

    const std::source_location& source_location() const noexcept override { return _source_location; }
          std::string_view      message()         const noexcept override { return _message; }
          std::string&          message()               noexcept override { return _message; }

    /// @brief Set a new message for this exception
    ///
    /// @param message The new message
    exception& with_message(std::string_view custom_message) {
        message() += "\nCustom message: " + std::string(custom_message);
        return *this;
    }
};

class unknown_exception : public base_exception<error_category_t> {
public:
    using error_category_t = tools::exception::error_category_t;
    static constexpr error_category_t error_category = error_category_t::unknown;

    enum class error_code_t {
        unknown = 0
    };
};

template<>
consteval std::string_view default_error_message<unknown_exception::error_code_t>(const unknown_exception::error_code_t& code) {
    switch (code) {
    case unknown_exception::error_code_t::unknown:
        return "An unknown error occurred";

    default:
        return "???";
    }
}

template<>
consteval std::string_view to_string<unknown_exception::error_code_t>(const unknown_exception::error_code_t &code) {
    switch (code) {
    case unknown_exception::error_code_t::unknown:
        return "unknown";

    default:
        return "???";
    }
}

static_assert(concrete_exception<unknown_exception>);

using unknown_error = exception<unknown_exception, unknown_exception::error_code_t::unknown>;

} // namespace tools::exception

#define CPPTOOLS_THROW(exception_type, ...) throw exception_type(std::source_location::current(),  ##__VA_ARGS__ )

#endif//CPPTOOLS_EXCEPTION_EXCEPTION_HPP
