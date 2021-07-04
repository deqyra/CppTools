#ifndef CPPTOOLS__UTILITY__HASH_COMBINE_HPP
#define CPPTOOLS__UTILITY__HASH_COMBINE_HPP

#include <functional>

namespace cpptools
{

template <class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
}

}//namespace cpptools

#endif//CPPTOOLS__UTILITY__HASH_COMBINE_HPP