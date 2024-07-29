#ifndef CPPTOOLS_UTILITY_DETAIL_RANGE_BASE_HPP
#define CPPTOOLS_UTILITY_DETAIL_RANGE_BASE_HPP

#include <iterator>
#include <ranges>
#include <type_traits>

#include <cpptools/utility/type_traits.hpp>

namespace tools::ranges::detail {

constexpr inline bool _const     = ::tools::detail::_const;
constexpr inline bool _not_const = ::tools::detail::_not_const;

template<std::ranges::range Rng>
using iterator_of = decltype(std::ranges::begin(std::declval<Rng&>()));

template<std::ranges::range R>
using sentinel_of = decltype(::std::ranges::end(::std::declval<R&>()));

template<std::ranges::range Rng>
struct range_traits {
    static constexpr bool view          = std::ranges::view<Rng>;
    static constexpr bool input         = std::ranges::input_range<Rng>;
    static constexpr bool forward       = std::ranges::forward_range<Rng>;
    static constexpr bool bidirectional = std::ranges::bidirectional_range<Rng>;
    static constexpr bool random_access = std::ranges::random_access_range<Rng>;
    static constexpr bool contiguous    = std::ranges::contiguous_range<Rng>;
    static constexpr bool common        = std::ranges::common_range<Rng>;
    static constexpr bool sized         = std::ranges::sized_range<Rng>;

    using iterator           = iterator_of<Rng>;
    using iterator_value     = typename std::iterator_traits<iterator>::value_type;
    using iterator_reference = typename std::iterator_traits<iterator>::reference;
    using iterator_diff      = typename std::iterator_traits<iterator>::difference_type;
};

template<std::ranges::range Rng>
using iterator_concept_of = std::conditional_t<
    std::ranges::random_access_range<Rng>,
    std::random_access_iterator_tag,
    std::conditional_t<
        std::ranges::bidirectional_range<Rng>,
        std::bidirectional_iterator_tag,
        std::conditional_t<
            std::ranges::forward_range<Rng>,
            std::forward_iterator_tag,
            std::input_iterator_tag
        >
    >
>;

// Empty because C++20 iterators may not meet the requirements for C++17 InputIterators:
// by not defining an iterator category for this, we don't unnecessarily lie to C++17 algorithms
template<typename Rng>
struct iterator_category_base {};

// For ForwardIterators and stronger, a category can be safely provided
template<typename Rng>
    requires std::ranges::forward_range<Rng>
struct iterator_category_base<Rng> {
    using traits = range_traits<Rng>;
    using iterator_category = std::conditional_t<
        std::is_reference_v<typename traits::iterator_reference>,
        std::conditional_t<
            std::derived_from<category_of<iterator_of<Rng>>, std::contiguous_iterator_tag>,
            std::random_access_iterator_tag,
            category_of<iterator_of<Rng>>
        >,
        std::input_iterator_tag
    >;
};

/// @brief Implementation base for a view iterator
/// @note How to use:
/// - define an iterator type which publicly inherits from this class
/// - define member functions that shadow the default implementation as needed
template<typename Derived, bool Const, typename Parent, typename Adapted>
class iterator_base : public detail::iterator_category_base<maybe_const<Const, Adapted>> {
protected:
    using parent_range     = maybe_const<Const, Parent>;
    using adapted_range    = maybe_const<Const, Adapted>;
    using adapted_iterator = iterator_of<adapted_range>;
    using traits           = range_traits<adapted_range>;

    [[nodiscard]] static constexpr const iterator_base& as_base(const Derived& it) {
        return static_cast<const iterator_base&>(it);
    }

    [[nodiscard]] static constexpr iterator_base& as_base(Derived& it) {
        return static_cast<iterator_base&>(it);
    }

    [[nodiscard]] static constexpr const Derived& as_derived(const iterator_base& it) {
        return static_cast<const Derived&>(it);
    }

    [[nodiscard]] static constexpr Derived& as_derived(iterator_base& it) {
        return static_cast<Derived&>(it);
    }

public:
    using difference_type  = typename std::iterator_traits<adapted_iterator>::difference_type;

    // C++20 iterator concept opt-in
    using iterator_concept = detail::iterator_concept_of<adapted_range>;

    constexpr iterator_base() requires std::default_initializable<adapted_iterator> = default;
    constexpr iterator_base(parent_range& parent, adapted_iterator current)
        noexcept(std::is_nothrow_move_constructible_v<adapted_iterator>)
        : _parent{std::addressof(parent)}
        , _current{std::move(current)}
    {

    }

    constexpr iterator_base(iterator_base<Derived, !Const, Parent, Adapted> it)
        noexcept(std::is_nothrow_constructible_v<iterator_of<const Adapted>, iterator_of<Adapted>>)
        requires Const && std::convertible_to<iterator_of<Parent>, adapted_iterator>
        : _parent(it._parent)
        , _current(std::move(it._current))
    {

    }

    [[nodiscard]] constexpr decltype(auto) operator*() const {
        return *_current;
    }

    constexpr Derived& operator++() noexcept(noexcept(++_current)) {
        ++_current;
        return as_derived(*this);
    }

    constexpr decltype(auto) operator++(int)
        noexcept(noexcept(++_current) && (!traits::forward || std::is_nothrow_copy_constructible_v<Derived>))
    {
        if constexpr (traits::forward) {
            auto tmp = as_derived(*this);
            ++*this;
            return tmp;
        } else {
            ++*this;
        }
    }

    constexpr Derived& operator--() noexcept(noexcept(--_current))
        requires traits::bidirectional
    {
        --_current;
        return as_derived(*this);
    }

    constexpr Derived operator--(int)
        noexcept(noexcept(--_current) && std::is_nothrow_copy_constructible_v<Derived>)
        requires traits::bidirectional
    {
        auto tmp = as_derived(*this);
        --*this;
        return tmp;
    }

    constexpr Derived& operator+=(const difference_type off)
        noexcept(noexcept(_current += off))
        requires traits::random_access
    {
        _current += off;
        return as_derived(*this);
    }

    constexpr Derived& operator-=(const difference_type off)
        noexcept(noexcept(_current -= off))
        requires traits::random_access
    {
        _current -= off;
        return as_derived(*this);
    }

    [[nodiscard]] constexpr decltype(auto) operator[](const difference_type idx) const
        noexcept(noexcept(_current[idx]))
        requires traits::random_access
    {
        return _current[idx];
    }

    [[nodiscard]] friend constexpr bool operator==(const iterator_base& left, const iterator_base& right)
        noexcept(noexcept(left._current == right._current))
        requires std::equality_comparable<adapted_iterator>
    {
        return left._current == right._current;
    }

    [[nodiscard]] friend constexpr bool operator<(const iterator_base& left, const iterator_base& right)
        noexcept(noexcept(left._current < right._current))
        requires traits::random_access
    {
        return left._current < right._current;
    }

    [[nodiscard]] friend constexpr bool operator>(const iterator_base left, const iterator_base& right)
        noexcept(noexcept(right < left))
        requires traits::random_access
    {
        return right < left;
    }

    [[nodiscard]] friend constexpr bool operator<=(const iterator_base& left, const iterator_base& right)
        noexcept(noexcept(!(right < left)))
        requires traits::random_access
    {
        return !(right < left);
    }

    [[nodiscard]] friend constexpr bool operator>=(const iterator_base& left, const iterator_base& right)
        noexcept(noexcept(!(left < right)))
        requires traits::random_access
    {
        return !(left < right);
    }

    [[nodiscard]] friend constexpr auto operator<=>(const iterator_base& left, const iterator_base& right)
        noexcept(noexcept(left._current <=> right._current))
        requires traits::random_access && std::three_way_comparable<adapted_iterator>
    {
        return left._current <=> right._current;
    }

    [[nodiscard]] friend constexpr Derived operator+(iterator_base it, const difference_type off)
        noexcept(noexcept(it._current += off))
        requires traits::random_access
    {
        it._current += off;
        return it;
    }

    [[nodiscard]] friend constexpr Derived operator+(const difference_type off, iterator_base it)
        noexcept(noexcept(it._current += off))
        requires traits::random_access
    {
        it._current += off;
        return it;
    }

    [[nodiscard]] friend constexpr Derived operator-(iterator_base it, const difference_type off)
        noexcept(noexcept(it._current -= off))
        requires traits::random_access
    {
        it._current -= off;
        return it;
    }

    [[nodiscard]] friend constexpr difference_type operator-(const iterator_base& left, const iterator_base& right)
        noexcept(noexcept(left._current - right._current))
        requires std::sized_sentinel_for<adapted_iterator, adapted_iterator>
    {
        return left._current - right._current;
    }

    [[nodiscard]] constexpr adapted_iterator base() const
        noexcept(std::is_nothrow_copy_constructible_v<adapted_iterator>)
    {
        return _current;
    }

protected:
    parent_range* _parent;
    adapted_iterator _current;
};

/// @brief Implementation base for a view sentinel
/// @note How to use:
/// - define a sentinel type which publicly inherits from this class
/// - define member functions that shadow the default implementation as needed
template <typename Derived, bool Const, typename Parent, typename Adapted>
class sentinel_base {
private:
    using _parent_range  = maybe_const<Const, Parent>;
    using _adapted_range = maybe_const<Const, Adapted>;

    template <bool OtherConst>
    using _iter_base = iterator_base<Derived, OtherConst, Parent, Adapted>;

    template <bool OtherConst>
    using _maybe_const_iter = iterator_of<maybe_const<OtherConst, Adapted>>;

    sentinel_of<_adapted_range> _last;

public:
    sentinel_base() = default;
    constexpr explicit sentinel_base(sentinel_of<_adapted_range> last) noexcept(
        std::is_nothrow_move_constructible_v<sentinel_of<_adapted_range>>)
        : _last(std::move(last))
    {

    }

    constexpr sentinel_base(sentinel_base<Derived, !Const, Parent, Adapted> sentinel)
        noexcept(std::is_nothrow_constructible_v<sentinel_of<_adapted_range>, sentinel_of<Adapted>>)
        requires Const && std::convertible_to<sentinel_of<Adapted>, sentinel_of<_adapted_range>>
        : _last(std::move(sentinel._last))
    {

    }

    [[nodiscard]] constexpr sentinel_of<_adapted_range> base() const
        noexcept(std::is_nothrow_copy_constructible_v<sentinel_of<_adapted_range>>)
    {
        return _last;
    }

    template <bool OtherConst>
        requires std::sentinel_for<sentinel_of<_adapted_range>, _maybe_const_iter<OtherConst>>
    [[nodiscard]] friend constexpr bool operator==(const _iter_base<OtherConst>& left, const sentinel_base& right)
        noexcept(noexcept(left._current == right._last))
    {
        return left._current == right._last;
    }

    template <bool OtherConst>
        requires std::sized_sentinel_for<sentinel_of<_adapted_range>, _maybe_const_iter<OtherConst>>
    [[nodiscard]] friend constexpr range_traits<maybe_const<OtherConst, Adapted>>::iter_difference
        operator-(const _iter_base<OtherConst>& left, const sentinel_base& right)
        noexcept(noexcept(left._current - right._last))
    {
        return left._current - right._last;
    }

    template <bool OtherConst>
        requires std::sized_sentinel_for<sentinel_of<_adapted_range>, _maybe_const_iter<OtherConst>>
    [[nodiscard]] friend constexpr range_traits<maybe_const<OtherConst, Adapted>>::iter_difference
        operator-(const sentinel_base& left, const _iter_base<OtherConst>& right)
        noexcept(noexcept(left._last - right._current))
    {
        return left._last - right._current;
    }
};

/// @brief Implementation base for a view
/// @note How to use:
/// - define a view type which publicly inherits from this class
/// - define a type named iterator (which possibly inherits from detail::iterator_base)
/// - define a type named sentinel (which possibly inherits from detail::sentinel_base)
/// - define member functions that shadow the default implementation as needed
template<std::ranges::input_range Rng, typename Vw, template<bool> typename Iterator, template<bool> typename Sentinel>
    requires std::ranges::view<Rng>
class view_impl : public std::ranges::view_interface<Vw> {
protected:
    Rng _range;

    template<bool Const>
    using traits = range_traits<maybe_const<Const, Rng>>;

    template<bool Const>
    using adapted_iterator = typename traits<Const>::iterator;

    template<bool Const>
    using adapted_reference = typename traits<Const>::iterator_reference;

public:
    template<bool Const>
    using reference = decltype(*(std::declval<adapted_reference<Const>>()));

    template<bool Const>
    using value_type = std::remove_reference_t<reference<Const>>;

    view_impl() requires std::default_initializable<Rng> = default;
    view_impl(Rng range) noexcept(std::is_nothrow_move_constructible_v<Rng>) :
        _range(std::move(range))
    {

    }

    [[nodiscard]] constexpr Rng base() const&
        noexcept(std::is_nothrow_copy_constructible_v<Rng>)
        requires std::copy_constructible<Rng>
    {
        return _range;
    }
    
    [[nodiscard]] constexpr Rng base() &&
        noexcept(std::is_nothrow_move_constructible_v<Rng>)
    {
        return std::move(_range);
    }

    Iterator<_not_const> begin()
        noexcept(noexcept(std::ranges::begin(_range)) && std::is_nothrow_move_constructible_v<adapted_iterator<false>>)
    {
        return Iterator<_not_const>(static_cast<Vw&>(*this), std::begin(_range));
    }

    Iterator<_const> begin() const
        noexcept(noexcept(std::ranges::begin(_range)) && std::is_nothrow_move_constructible_v<adapted_iterator<true>>)
    {
        return Iterator<_const>(static_cast<const Vw&>(*this), std::begin(_range));
    }

    [[nodiscard]] constexpr auto end()
        noexcept(noexcept(std::ranges::end(_range)) && std::is_nothrow_move_constructible_v<decltype(std::ranges::end(_range))>)
    {
        if constexpr (traits<_not_const>::common) {
            return Iterator<_not_const>{static_cast<Vw&>(*this), std::ranges::end(_range)};
        } else {
            return Sentinel<_not_const>{std::ranges::end(_range)};
        }
    }

    [[nodiscard]] constexpr auto end() const
        noexcept(noexcept(std::ranges::end(_range)) && std::is_nothrow_move_constructible_v<decltype(std::ranges::end(_range))>)
        requires std::ranges::range<const Rng>
    {
        if constexpr (traits<_const>::common) {
            return Iterator<_const>{static_cast<const Vw&>(*this), std::ranges::end(_range)};
        } else {
            return Sentinel<_const>{std::ranges::end(_range)};
        }
    }

    // Implemented by std::ranges::view_interface:
    // - empty
    // - cbegin
    // - cend
    // - operator bool
    // - data
    // - size
    // - front
    // - back
    // - operator[]
    // - const variants of the above
};

/// @brief Implementation base for a range adaptor closure object
template<typename Cpo, typename... Args>
class closure : public std::ranges::range_adaptor_closure<closure<Cpo, Args...>> {
public:
    static_assert((std::same_as<std::decay_t<Args>, Args> && ...));
    static_assert(std::is_empty_v<Cpo> && std::is_default_constructible_v<Cpo>);

    template<class... Ts>
        requires (std::same_as<std::decay_t<Ts>, Args> && ...)
    constexpr explicit closure(Ts&&... _Args)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<Args, Ts>...>)
        : _adaptor_args(std::forward<Ts>(_Args)...)
    {

    }

    using idx_seq = std::index_sequence_for<Args...>;

    template<typename Rng>
        requires std::invocable<Cpo, Rng, Args&...>
    constexpr decltype(auto) operator()(this auto&& self, Rng&& range)
        noexcept(noexcept(self._call(std::forward<Rng>(range), idx_seq{})))
    {
        return self._call(std::forward<Rng>(range), idx_seq{});
    }

private:
    template<typename Rng, std::size_t... Idx>
    constexpr decltype(auto) _call(this auto&& self, Rng&& range, std::index_sequence<Idx...>)
        noexcept(noexcept(Cpo{}(std::forward<Rng>(range), std::get<Idx>(self._adaptor_args)...)))
    {
        return Cpo{}(std::forward<Rng>(range), std::get<Idx>(self._adaptor_args)...);
    }

    std::tuple<Args...> _adaptor_args;
};

} // namespace tools::ranges::detail

#endif//CPPTOOLS_UTILITY_DETAIL_RANGE_BASE_HPP