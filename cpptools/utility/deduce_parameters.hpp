#ifndef CPPTOOLS_UTILITY_DEDUCE_PARAMETERS_HPP
#define CPPTOOLS_UTILITY_DEDUCE_PARAMETERS_HPP

#include <utility>

namespace tools {

template<typename R, typename... Args>
R deduce_return_type(R(*)(Args...));

template<typename T, typename R, typename... Args>
R deduce_return_type(R(T::*)(Args...)const);

template<typename T, typename R, typename... Args>
R deduce_return_type(R(T::*)(Args...));

template<typename R, typename... Args>
std::tuple<Args...> deduce_parameters(R(*)(Args...));

template<typename T, typename R, typename... Args>
std::tuple<Args...> deduce_parameters(R(T::*)(Args...)const);

template<typename T, typename R, typename... Args>
std::tuple<Args...> deduce_parameters(R(T::*)(Args...));

template<
    typename F,
    typename R    = decltype(deduce_return_type(&F::operator())),
    typename Args = decltype(deduce_parameters(&F::operator()))
>
Args deduce_parameters(F&& f);

} // namespace tools

#endif//CPPTOOLS_UTILITY_DEDUCE_PARAMETERS_HPP