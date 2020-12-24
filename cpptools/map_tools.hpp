#ifndef CPPTOOLS__MULTIMAP_TOOLS_HPP
#define CPPTOOLS__MULTIMAP_TOOLS_HPP

#include <map>
#include <unordered_map>
#include <utility>

template<typename Key, typename Val, typename Hash, typename Pred, typename Alloc, template<typename Key, typename Val, typename Hash, typename Pred, typename Alloc> typename T>
bool mapContainsPair(const T<Key, Val, Hash, Pred, Alloc>& map, const std::pair<const Key, Val>& pair)
{
    using Iter = T<Key, Val, Hash, Pred, Alloc>::iterator;
    std::pair<Iter, Iter> range = map.equal_range(pair.first);
    for (Iter it = range.first; it != range.second; it++)
    {
        if (it->second == pair.second) return true;
    }

    return false;
}

#endif//CPPTOOLS__MULTIMAP_TOOLS_HPP