#ifndef CPPTOOLS_API_HPP
#define CPPTOOLS_API_HPP

#if   defined CPPTOOLS_DLL_IMPORT
#   define CPPTOOLS_API __declspec(dllimport)
#elif defined CPPTOOLS_DLL_EXPORT
#   define CPPTOOLS_API __declspec(dllexport)
#else
#   define CPPTOOLS_API
#endif

#endif//CPPTOOLS_API_HPP