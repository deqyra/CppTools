#ifndef CPPTOOLS_EXCEPTION_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_EXCEPTION_HPP

#include <ostream>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <cpptools/api.hpp>

#include "error_category.hpp"

namespace tools::exception
{

/// Exception model in cpptools:
///
/// - In the following, T is a "concrete" exception type but is not meant to be throwable in and of itself
/// - Instead, T represents an *error category*, a category of throwable exception classes
/// - The possible error categories are grouped as literals in enum type error_category
/// - Any T must satisfy the concrete_exception concept, which has the following characteristics:
///     - T inherits from base_exception
///     - T provides a public, static constexpr member named "error_category", of enum type error_category
///     - The value of T::error_category is a literal that represents the error category which T is meant for
///     - T provides a public enum type named "ecode"
///     - The literals in T::ecode describe error cases in the category of errors that T represents
///     - A specialization of tools::exception::default_error_message for T::ecode is visible alongside T
///     - A specialization of tools::exception::to_string for T::ecode is visible alongside T
/// - Every exception class meant to be throwable must be a specialization of the `exception` template, which:
///     - uses CRTP to inherit from a type T as described above
///     - is also template-parameterized on a specific error code literal in T::error_code

class base_exception;

template<typename E>
consteval std::string_view default_error_message(const E& code) = delete;

template<typename E>
consteval std::string_view to_string(const E& code) = delete;

template<typename T>
concept concrete_exception = requires {
    std::is_base_of_v<base_exception, T>;
    std::is_enum_v<typename T::ecode>;
    std::is_same_v<decltype(T::error_category), error_category>;

    { default_error_message<typename T::ecode>(static_cast<typename T::ecode>(0)) }
        -> std::convertible_to<std::string_view>;
    {             to_string<typename T::ecode>(static_cast<typename T::ecode>(0)) }
        -> std::convertible_to<std::string_view>;
};

/// @brief Specialize to instantiate concrete exception classes.
///
/// @tparam T Parent exception class. Must satisfy concrete_exception.
/// @tparam code Error code related to T's handled error category.
template<concrete_exception T, typename T::ecode code>
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
        _message(default_error_message(code))
    {
    }

    exception(const exception &e) = default;

    // Overrides from base_exception

    error_category   category()        const override { return T::error_category; }
    std::size_t      error_code()      const override { return static_cast<std::size_t>(code); }
    std::string_view ecode_to_string() const override { return tools::exception::to_string(code); }

    const std::source_location& source_location() const override { return _source_location; }
          std::string_view      message()         const override { return _message; }
          std::string&          message()               override { return _message; }

    /// @brief Set a new message for this exception
    ///
    /// @param message The new message
    exception& with_message(std::string_view message) {
        _message = message;
        return *this;
    }
};

/// @brief Base class which has to be inherited by all new exception classes
class base_exception : public std::exception {
public:
    base_exception() = default;
    virtual ~base_exception() = default;

    virtual error_category   category()        const = 0;
    virtual std::size_t      error_code()      const = 0;
    virtual std::string_view ecode_to_string() const = 0;

    virtual const std::source_location& source_location() const = 0;
    virtual       std::string_view      message()         const = 0;
    virtual       std::string&          message()               = 0;

    CPPTOOLS_API virtual const char* what() const override;

    CPPTOOLS_API virtual std::string_view to_string() const;

protected:
    mutable std::string _str;
};

CPPTOOLS_API std::ostream& operator<<(std::ostream& out, const base_exception& e);

class unknown_exception : public base_exception {
public:
    static constexpr error_category error_category = error_category::unknown;

    enum class ecode {
        unknown = 0
    };
};

template<>
consteval std::string_view default_error_message<unknown_exception::ecode>(const unknown_exception::ecode &code) {
    switch (code) {
    case unknown_exception::ecode::unknown:
        return "An unknown error occurred";

    default:
        return "???";
    }
}

template<>
consteval std::string_view to_string<unknown_exception::ecode>(const unknown_exception::ecode &code) {
    switch (code) {
    case unknown_exception::ecode::unknown:
        return "unknown";

    default:
        return "???";
    }
}

using unknown_error = exception<unknown_exception, unknown_exception::ecode::unknown>;

} // namespace tools::exception

#define CPPTOOLS_THROW(exception_type, ...) throw exception_type(std::source_location::current(),  ##__VA_ARGS__ )

#endif//CPPTOOLS_EXCEPTION_EXCEPTION_HPP
