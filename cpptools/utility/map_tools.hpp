#ifndef CPPTOOLS__UTILITY__MULTIMAP_TOOLS_HPP
#define CPPTOOLS__UTILITY__MULTIMAP_TOOLS_HPP

#include <map>
#include <unordered_map>
#include <utility>

namespace cpptools
{

template<template<typename, typename, typename, typename, typename> typename T, typename Key, typename Val, typename Hash, typename Pred, typename Alloc>
//template<, template<typename Key, typename Val, typename Hash, typename Pred, typename Alloc> typename T>
bool mapContainsPair(const T<Key, Val, Hash, Pred, Alloc>& map, const std::pair<const Key, Val>& pair)
{
    using Iter = typename T<Key, Val, Hash, Pred, Alloc>::const_iterator;
    std::pair<Iter, Iter> range = map.equal_range(pair.first);
    for (Iter it = range.first; it != range.second; it++)
    {
        if (it->second == pair.second) return true;
    }

    return false;
}

}//namespace cpptools

#endif//CPPTOOLS__UTILITY__MULTIMAP_TOOLS_HPP