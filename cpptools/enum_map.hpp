#ifndef CPPTOOLS__ENUM_MAP_HPP
#define CPPTOOLS__ENUM_MAP_HPP

#include <map>
#include <unordered_map>

namespace std
{
    class EnumHash
    {
        public:
            template <typename T>
            std::size_t operator()(T t) const
            {
                return static_cast<std::size_t>(t);
            }
    };

    template<typename Key, typename T>
    using unordered_enum_map = unordered_map<Key, T, EnumHash>;

    template<typename Key, typename T>
    using enum_map = map<Key, T, EnumHash>;

    template<typename Key, typename T>
    using unordered_enum_multimap = unordered_multimap<Key, T, EnumHash>;

    template<typename Key, typename T>
    using enum_multimap = multimap<Key, T, EnumHash>;
}

#endif//CPPTOOLS__ENUM_MAP_HPP