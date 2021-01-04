#ifndef CPPTOOLS__ENUM_MAP_HPP
#define CPPTOOLS__ENUM_MAP_HPP

#include <map>
#include <type_traits>
#include <unordered_map>

namespace CppTools
{

class enum_hash
{
    public:
        template <typename T>
        std::size_t operator()(T t) const
        {
            return static_cast<std::size_t>(t);
        }
};

template<typename Key, typename T>
using unordered_enum_map = std::unordered_map<Key, T, enum_hash>;

template<typename Key, typename T>
using enum_map = std::map<Key, T, enum_hash>;

template<typename Key, typename T>
using unordered_enum_multimap = std::unordered_multimap<Key, T, enum_hash>;

template<typename Key, typename T>
using enum_multimap = std::multimap<Key, T, enum_hash>;

}//namespace CppTools

#endif//CPPTOOLS__ENUM_MAP_HPP