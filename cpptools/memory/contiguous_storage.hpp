#ifndef CPPTOOLS_MEMORY_CONTIGUOUS_STORAGE_HPP
#define CPPTOOLS_MEMORY_CONTIGUOUS_STORAGE_HPP

#include <array>
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

public:
    constexpr uninitialized_storage_proxy(T* ptr) : _ptr(ptr) {}

    static constexpr bool has_implicit_lifetime = true;

    constexpr T& value() {
        return *_ptr;
    }

    constexpr const T& value() const {
        return *_ptr;
    }

    template<typename U>
    constexpr uninitialized_storage_proxy& operator=(U&& v) {
        *_ptr = v;
        return *this;
    }
};

template<typename T, std::size_t N>
class uninitialized_storage_proxy<T[N]> {
    using array_t = T[N];
    array_t* _ptr;

public:
    constexpr uninitialized_storage_proxy(array_t* ptr) : _ptr(ptr) {}

    static constexpr bool has_implicit_lifetime = false;

    constexpr array_t& value() {
        return *_ptr;
    }

    constexpr const array_t& value() const {
        return *_ptr;
    }

    template<typename... Args>
    constexpr T& construct(std::size_t index, Args&&... args) {
        std::construct_at(&(_ptr[0]) + index, std::forward<Args>(args)...);
        return _ptr[index];
    }

    constexpr void destroy(std::size_t index) {
        std::destroy_at(&(_ptr[0]) + index);
    }

    constexpr T& operator[](std::size_t index) {
        return (*_ptr)[index];
    }

    constexpr const T& operator[](std::size_t index) const {
        return (*_ptr)[index];
    }
};

template<implicit_lifetime_type T, std::size_t N>
class uninitialized_storage_proxy<T[N]> {
    using array_t = T[N];
    array_t* _ptr;

public:
    uninitialized_storage_proxy(array_t* ptr) : _ptr(ptr) {}
    
    static constexpr bool has_implicit_lifetime = true;

    constexpr array_t& value() {
        return *_ptr;
    }

    constexpr const array_t& value() const {
        return *_ptr;
    }

    constexpr T& operator[](std::size_t index) {
        return (*_ptr)[index];
    }

    constexpr const T& operator[](std::size_t index) const {
        return (*_ptr)[index];
    }
};

template<std::size_t N>
consteval std::array<std::size_t, N> accumulate_leftwards(const std::array<std::size_t, N>& array) {
    std::array<std::size_t, N> result;
    std::size_t sum_of_previous = 0;
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = sum_of_previous;
        sum_of_previous += array[i];
    }
    return result;
}

template<typename... Ts>
consteval std::array<std::size_t, sizeof...(Ts)> accumulated_offsets() {
    return accumulate_leftwards<sizeof...(Ts)>({ sizeof(Ts)... });
}

template<typename... Ts, std::size_t... I>
constexpr std::tuple<uninitialized_storage_proxy<Ts>...> uninitialized_storage_from_base_and_offsets_helper(std::byte* base, const std::array<std::size_t, sizeof...(Ts)>& offsets, std::index_sequence<I...>) {
    return { uninitialized_storage_proxy<Ts>(std::launder(reinterpret_cast<Ts*>(base + offsets[I])))... };
}

template<typename... Ts>
constexpr std::tuple<uninitialized_storage_proxy<Ts>...> uninitialized_storage_from_base_and_offsets(std::byte* base, const std::array<std::size_t, sizeof...(Ts)>& offsets) {
    return uninitialized_storage_from_base_and_offsets_helper<Ts...>(base, offsets, std::make_index_sequence<sizeof...(Ts)>{});
}

}

/// @brief A wrapper for that allocates a contiguous are of unitialized storage
/// large enough to contain one instance of every type in Ts, laid out in sequence
template<implicit_lifetime_type... Ts>
class contiguous_storage {
public:
    static constexpr std::size_t Size = (sizeof(Ts) + ... );

private:
    alignas(std::max({alignof(Ts)...})) std::byte _storage[Size];
    std::tuple<detail::uninitialized_storage_proxy<Ts>...> _accessors;

    template<typename T>
    using proxy = detail::uninitialized_storage_proxy<T>;

public:
    static constexpr std::array<std::size_t, sizeof...(Ts)> Sizes = { sizeof(Ts)... };
    static constexpr std::array<std::size_t, sizeof...(Ts)> Offsets = detail::accumulated_offsets<Ts...>();

    static constexpr bool has_implicit_lifetime = (proxy<Ts>::has_implicit_lifetime && ...);

    constexpr contiguous_storage() :
        _storage{},
        _accessors(detail::uninitialized_storage_from_base_and_offsets<Ts...>(&_storage[0], Offsets))
    {

    }

    template<typename T>
    proxy<T>& get() {
        return std::get<proxy<T>>(_accessors);
    }

    template<typename T>
    const proxy<T>& get() const {
        return std::get<proxy<T>>(_accessors);
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