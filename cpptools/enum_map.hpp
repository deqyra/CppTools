#ifndef CPPTOOLS__ENUM_MAP_HPP
#define CPPTOOLS__ENUM_MAP_HPP

#include <map>
#include <type_traits>
#include <unordered_map>

namespace std
{
    template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
    class hash
    {
        public:
            std::size_t operator()(T t) const
            {
                return static_cast<std::size_t>(t);
            }
    };

    template<typename Key, typename T>
    using unordered_enum_map = unordered_map<Key, T>;
}

#endif//CPPTOOLS__ENUM_MAP_HPP