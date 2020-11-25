#ifndef TOOLS__ENUM_MAP_HPP
#define TOOLS__ENUM_MAP_HPP

#include <unordered_map>

namespace std
{
    struct EnumClassHash
    {
        template <typename T>
        std::size_t operator()(T t) const
        {
            return static_cast<std::size_t>(t);
        }
    };

    template<typename Key, typename T>
    using enum_map = unordered_map<Key, T, EnumClassHash>;
}

#endif//TOOLS__ENUM_MAP_HPP