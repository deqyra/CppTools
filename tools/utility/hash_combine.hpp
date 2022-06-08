#ifndef TOOLS__UTILITY__HASH_COMBINE_HPP
#define TOOLS__UTILITY__HASH_COMBINE_HPP

#include <functional>

namespace tools
{

template <class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
}

} // namespace tools

#endif//TOOLS__UTILITY__HASH_COMBINE_HPP