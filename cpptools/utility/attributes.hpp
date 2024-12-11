#ifndef CPPTOOLS_UTILITY_ATTRIBUTES_HPP
#define CPPTOOLS_UTILITY_ATTRIBUTES_HPP

#ifdef _MSC_VER
#   define NO_UNIQUE_ADDR [[msvc::no_unique_address]]
#else
#   define NO_UNIQUE_ADDR [[no_unique_address]]
#endif

#endif//CPPTOOLS_UTILITY_ATTRIBUTES_HPP