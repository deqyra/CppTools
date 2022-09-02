#ifndef CPPTOOLS__UTILITY__MAP_HPP
#define CPPTOOLS__UTILITY__MAP_HPP

#include <utility>

namespace tools::map
{

template<
    template<typename...> typename T,
    typename Key, typename Val, typename... ArgTypes
>
bool contains_pair(
    const T<Key, Val, ArgTypes...>& map,
    const std::pair<const Key, Val>& pair
)
{
    auto range = map.equal_range(pair.first);
    for (auto it = range.first; it != range.second; it++)
    {
        if (it->second == pair.second) return true;
    }

    return false;
}

} // namespace tools::map

#endif//CPPTOOLS__UTILITY__MAP_HPP