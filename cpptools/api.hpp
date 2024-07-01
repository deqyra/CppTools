#ifndef CPPTOOLS_API_HPP
#define CPPTOOLS_API_HPP

#if   defined CPPTOOLS_DLL_IMPORT && defined(_WIN32)
#   define CPPTOOLS_API __declspec(dllimport)
#elif defined CPPTOOLS_DLL_EXPORT && defined(_WIN32)
#   define CPPTOOLS_API __declspec(dllexport)
#else
#   define CPPTOOLS_API
#endif

#endif//CPPTOOLS_API_HPP