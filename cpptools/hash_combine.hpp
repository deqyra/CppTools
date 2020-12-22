#ifndef CPPTOOLS__HASH_COMBINE_HPP
#define CPPTOOLS__HASH_COMBINE_HPP

#include <functional>

template <class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
}

#endif//CPPTOOLS__HASH_COMBINE_HPP