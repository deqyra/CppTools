#ifndef CPPTOOLS___INTERNALS__UTILITY_MACROS_HPP
#define CPPTOOLS___INTERNALS__UTILITY_MACROS_HPP

#include <sstream>

#if defined(_MSC_VER)
# define PLATFORM_PRETTY_FUNCTION __FUNCSIG__
#elif defined(__GNUG__)
# define PLATFORM_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__clang__)
# define PLATFORM_PRETTY_FUNCTION __builtin_FUNCTION
#else
# define PLATFORM_PRETTY_FUNCTION __func__
#endif

#define PREPEND_FUNCTION(message)                                              \
    [=](){                                                                     \
        ::std::ostringstream oss;                                              \
        oss << PLATFORM_PRETTY_FUNCTION << ": " << message;                    \
        return oss.str();                                                      \
    }()

#ifdef __has_cpp_attribute
# if __has_cpp_attribute(assume) >= 202207L
#   define CPPTOOLS_ASSUME(...) [[assume(__VA_ARGS__)]]
# endif
#endif

#ifndef CPPTOOLS_ASSUME
# if defined(__clang__)
#   define CPPTOOLS_ASSUME(...) __builtin_assume(__VA_ARGS__);
# elif defined(_MSC_VER)
#   define CPPTOOLS_ASSUME(...) __assume(__VA_ARGS__);
# elif defined(__GNUC__)
#   if __GNUC__ >= 13
#     define CPPTOOLS_ASSUME(...) __attribute__((__assume__(__VA_ARGS__)))
#   endif
# endif
#endif

#if !defined(CPPTOOLS_ASSUME) && CPPTOOLS_ALLOW_EVALUATING_ASSUMPTIONS != 0
# if __cpp_lib_unreachable >= 202202L
#   include <utility>
#   define CPPTOOLS_ASSUME(...) if (!(__VA_ARGS__)) ::std::unreachable();
# endif
#endif

#endif//CPPTOOLS___INTERNALS__UTILITY_MACROS_HPP