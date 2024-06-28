#ifndef CPPTOOLS_INTERNALS_UTILITY_MACROS_HPP
#define CPPTOOLS_INTERNALS_UTILITY_MACROS_HPP

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

#endif//CPPTOOLS_INTERNALS_UTILITY_MACROS_HPP