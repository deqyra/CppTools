#ifndef CPPTOOLS_MEMORY_CONTIGUOUS_STORAGE_HPP
#define CPPTOOLS_MEMORY_CONTIGUOUS_STORAGE_HPP

#include <cstddef>
#include <memory>
#include <tuple>
#include <utility>

#include <cpptools/utility/concepts.hpp>

namespace tools {

template<implicit_lifetime_type... Ts>
class contiguous_storage;

namespace detail {

template<implicit_lifetime_type T>
class uninitialized_storage_proxy {
    T* _ptr;

    template<implicit_lifetime_type... Ts>
    friend class contiguous_storage;

    uninitialized_storage_proxy(T* ptr) : _ptr(ptr) {}

public:
    static constexpr bool has_implicit_lifetime = true;

    T& get() {
        return *_ptr;
    }

    T& cget() const {
        return *_ptr;
    }

    const T& get() const {
        return *_ptr;
    }
    
    operator T&() {
        return get();
    }

    operator const T&() const {
        return get();
    }

    uninitialized_storage_proxy& operator=(T v) {
        *_ptr = std::move(v);
        return *this;
    }
};

template<typename T, std::size_t N>
class uninitialized_storage_proxy<T[N]> {
    T* _ptr;

    bool _initialized[N];

    template<implicit_lifetime_type... Ts>
    friend class contiguous_storage;

    constexpr uninitialized_storage_proxy(T* ptr) : _ptr(ptr), _initialized{false} {}

public:
    static constexpr bool has_implicit_lifetime = false;

    template<typename... Args>
    constexpr T& construct(std::size_t index, Args&&... args) {
        std::construct_at(_ptr + index, std::forward<Args>(args)...);
        _initialized[index] = true;
        return _ptr[index];
    }

    constexpr void destroy(std::size_t index) {
        std::destroy_at(_ptr + index);
        _initialized[index] = false;
    }

    constexpr T& operator[](std::size_t index) {
        return _ptr[index];
    }

    constexpr const T& operator[](std::size_t index) const {
        return _ptr[index];
    }

    constexpr T* begin() {
        return _ptr;
    }

    constexpr const T* begin() const {
        return _ptr;
    }

    constexpr const T* cbegin() const {
        return _ptr;
    }

    constexpr T* end() {
        return _ptr + N;
    }

    constexpr const T* end() const {
        return _ptr + N;
    }

    constexpr const T* cend() const {
        return _ptr + N;
    }

    constexpr uninitialized_storage_proxy& operator=(T v) {
        *_ptr = std::move(v);
        return *this;
    }
};

template<implicit_lifetime_type T, std::size_t N>
class uninitialized_storage_proxy<T[N]> {
    T* _ptr;

    template<implicit_lifetime_type... Ts>
    friend class contiguous_storage;

    uninitialized_storage_proxy(T* ptr) : _ptr(ptr) {}
    
public:
    static constexpr bool has_implicit_lifetime = true;

    T& operator[](std::size_t index) {
        return _ptr[index];
    }

    const T& operator[](std::size_t index) const {
        return _ptr[index];
    }

    uninitialized_storage_proxy& operator=(T v) {
        *_ptr = std::move(v);
        return *this;
    }  
};

template<std::size_t N>
consteval std::array<std::size_t, N> accumulate_leftwards(const std::array<std::size_t, N>& array) {
    auto result = array;
    for (std::size_t i = 1; i < N; ++i) {
        result[i] += result[i - 1];
    }
    return result;
}

template<typename... Ts>
consteval std::array<std::size_t, sizeof...(Ts)> accumulated_offsets() {
    return accumulate_leftwards({ sizeof(Ts)... });
}

template<typename... Ts, std::size_t... I>
consteval std::tuple<uninitialized_storage_proxy<Ts>...> uninitialized_storage_from_base_and_offsets_helper(void* base, const std::array<std::size_t, sizeof...(Ts)>& offsets, std::index_sequence<I...>) {
    return { uninitialized_storage_proxy<Ts>(base + offsets[I])... };
}

template<typename... Ts>
consteval std::tuple<uninitialized_storage_proxy<Ts>...> uninitialized_storage_from_base_and_offsets(void* base, const std::array<std::size_t, sizeof...(Ts)>& offsets) {
    return uninitialized_storage_from_base_and_offsets_helper(base, offsets, std::make_index_sequence<sizeof...(Ts)>{});
}

}

/// @brief A wrapper for that allocates a contiguous are of unitialized storage
/// large enough to contain one instance of every type in Ts, laid out in sequence
template<implicit_lifetime_type... Ts>
class contiguous_storage {
public:
    static constexpr std::size_t Size = (sizeof(Ts) + ... );

private:
    std::byte _storage[Size];
    std::tuple<detail::uninitialized_storage_proxy<Ts>...> _accessors;

public:
    static constexpr std::array<std::size_t, sizeof...(Ts)> Sizes = { sizeof(Ts)... };
    static constexpr std::array<std::size_t, sizeof...(Ts)> Offsets = detail::accumulated_offsets<Ts...>();

    static constexpr bool has_implicit_lifetime = (detail::uninitialized_storage_proxy<Ts>::has_implicit_lifetime && ...);

    contiguous_storage() :
        _storage{},
        _accessors(uninitialized_storage_from_base_and_offsets(&_storage[0], Offsets))
    {

    }

    template<typename T>
    T& get() {
        return std::get<T>(_accessors);
    }

    template<std::size_t I>
    std::tuple_element_t<I, decltype(_accessors)>& get() {
        return std::get<I>(_accessors);
    }

    std::byte* data() {
        return &_storage[0];
    }

    const std::byte* data() const {
        return &_storage[0];
    }
};

} // namespace tools

#endif//CPPTOOLS_MEMORY_CONTIGUOUS_STORAGE_HPP