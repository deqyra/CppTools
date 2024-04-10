#ifndef CPPTOOLS_UTILITY_HASH_COMBINE_HPP
#define CPPTOOLS_UTILITY_HASH_COMBINE_HPP

#include <utility>

namespace tools
{

struct hash_combine_params
{
    size_t initial_value;
    size_t left_shift;
    size_t right_shift;
};

inline constexpr hash_combine_params default_hash_combine_params = {
    .initial_value = 0x9e3779b9,
    .left_shift = 6,
    .right_shift = 2
};

template <
    class T,
    hash_combine_params params = default_hash_combine_params
>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s  ^= h(v)
        + params.initial_value
        + (s << params.left_shift)
        + (s >> params.right_shift);
}

} // namespace tools

#endif//CPPTOOLS_UTILITY_HASH_COMBINE_HPP