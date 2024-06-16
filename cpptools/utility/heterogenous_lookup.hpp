#ifndef CPPTOOLS_UTILITY_HETEROGENOUS_LOOKUP_HPP
#define CPPTOOLS_UTILITY_HETEROGENOUS_LOOKUP_HPP

#include <memory>

namespace tools
{

template<typename T, typename D>
struct unique_ptr_transparent_equality {
    using is_transparent = void;

    bool operator()(const std::unique_ptr<T, D>& ptr, T* raw) const {
        return ptr.get() == raw;
    }

    bool operator()(T* raw, const std::unique_ptr<T, D>& ptr) const {
        return (*this)(ptr, raw);
    }
};

template<typename U>
using unique_ptr_transparent_equality_t = 
    unique_ptr_transparent_equality<
        typename U::pointer,
        typename U::deleter_type
    >;

template<typename T, typename D>
struct unique_ptr_transparent_hash {
    using is_transparent = void;

    [[nodiscard]] std::size_t operator()(T* raw) const {
        return std::hash<T*>{}(raw);
    }

    [[nodiscard]] std::size_t operator()(const std::unique_ptr<T, D>& ptr) const {
        return (*this)(ptr.get());
    }
};

template<typename U>
using unique_ptr_transparent_hash_t = 
    unique_ptr_transparent_hash<
        typename U::pointer,
        typename U::deleter_type
    >;

} // namespace tools

#endif//CPPTOOLS_UTILITY_HETEROGENOUS_LOOKUP_HPP