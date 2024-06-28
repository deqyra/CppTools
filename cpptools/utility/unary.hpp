#ifndef CPPTOOLS_UTILITY_UNARY_HPP
#define CPPTOOLS_UTILITY_UNARY_HPP

namespace tools::unary {

template<typename T>
decltype(auto) dereference(T&& v) {
    return *v;
}

} // namespace tools::unary

#endif//CPPTOOLS_UTILITY_UNARY_HPP