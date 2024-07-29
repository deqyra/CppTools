#ifndef CPPTOOLS_UTILITY_RANGES_HPP
#define CPPTOOLS_UTILITY_RANGES_HPP

#include <ranges>
#include <type_traits>

#include <cpptools/utility/unary.hpp>
#include <cpptools/utility/type_traits.hpp>

#include "detail/range_base.hpp"

namespace tools::ranges {

template<std::ranges::input_range Rng> requires std::ranges::view<Rng>
class dereference_view;

template<std::ranges::input_range Rng> requires std::ranges::view<Rng>
class reverse_pair_view;

template<std::ranges::input_range Rng> requires std::ranges::view<Rng>
class reverse_map_pair_view;

template<std::ranges::input_range Rng, typename Cls, typename T> requires std::ranges::view<Rng>
class member_view;

namespace detail {

    template<bool Const, typename Rng>
    class dereference_view_iterator : public detail::iterator_base<dereference_view_iterator<Const, Rng>, Const, dereference_view<Rng>, Rng> {
        using _base_t = detail::iterator_base<dereference_view_iterator<Const, Rng>, Const, dereference_view<Rng>, Rng>;
        using traits = range_traits<maybe_const<Const, Rng>>;
        using adapted_reference = typename traits::iterator_reference;

    public:
        using reference = decltype(dereference(std::declval<adapted_reference>()));
        using value_type = std::remove_reference_t<reference>;
        // difference_type and (optionally) iterator_category provided by iterator_base

        using _base_t::_base_t;
        // dereference_view_iterator()
        // dereference_view_iterator(parent_range, adapted_iterator)
        // dereference_view_iterator(dereference_view_iterator)

        [[nodiscard]] constexpr decltype(auto) operator*() const
            noexcept(noexcept(dereference(_base_t::operator*())))
        {
            return dereference(_base_t::operator*());
        }
    };

    template<typename Rng>
    struct dereference_view_traits {
        template<bool Const>
        using iterator = dereference_view_iterator<Const, Rng>;

        template<bool Const>
        using sentinel = detail::sentinel_base<dereference_view_iterator<Const, Rng>, Const, dereference_view<Rng>, Rng>;
    };

    template<bool Const, typename Rng>
    class reverse_pair_view_iterator : public detail::iterator_base<reverse_pair_view_iterator<Const, Rng>, Const, reverse_pair_view<Rng>, Rng> {
        using _base_t = detail::iterator_base<reverse_pair_view_iterator<Const, Rng>, Const, reverse_pair_view<Rng>, Rng>;
        using traits = range_traits<maybe_const<Const, Rng>>;
        using adapted_reference = typename traits::iterator_reference;

    public:
        using reference = decltype(reverse_pair(std::declval<adapted_reference>()));
        using value_type = std::remove_reference_t<reference>;
        // difference_type and (optionally) iterator_category provided by iterator_base

        using _base_t::_base_t;
        // reverse_pair_view_iterator()
        // reverse_pair_view_iterator(parent_range, adapted_iterator)
        // reverse_pair_view_iterator(reverse_pair_view_iterator)

        [[nodiscard]] constexpr decltype(auto) operator*() const
            noexcept(noexcept(reverse_pair(_base_t::operator*())))
        {
            return reverse_pair(_base_t::operator*());
        }
    };

    template<typename Rng>
    struct reverse_pair_view_traits {
        template<bool Const>
        using iterator = reverse_pair_view_iterator<Const, Rng>;

        template<bool Const>
        using sentinel = detail::sentinel_base<reverse_pair_view_iterator<Const, Rng>, Const, reverse_pair_view<Rng>, Rng>;
    };

    template<bool Const, typename Rng>
    class reverse_map_pair_view_iterator : public detail::iterator_base<reverse_map_pair_view_iterator<Const, Rng>, Const, reverse_map_pair_view<Rng>, Rng> {
        using _base_t = detail::iterator_base<reverse_map_pair_view_iterator<Const, Rng>, Const, reverse_map_pair_view<Rng>, Rng>;
        using traits = range_traits<maybe_const<Const, Rng>>;
        using adapted_reference = typename traits::iterator_reference;

    public:
        using reference = decltype(reverse_map_pair(std::declval<adapted_reference>()));
        using value_type = std::remove_reference_t<reference>;
        // difference_type and (optionally) iterator_category provided by iterator_base

        using _base_t::_base_t;
        // reverse_map_pair_view_iterator()
        // reverse_map_pair_view_iterator(parent_range, adapted_iterator)
        // reverse_map_pair_view_iterator(reverse_map_pair_view_iterator)

        [[nodiscard]] constexpr decltype(auto) operator*() const
            noexcept(noexcept(reverse_map_pair(_base_t::operator*())))
        {
            return reverse_map_pair(_base_t::operator*());
        }
    };

    template<typename Rng>
    struct reverse_map_pair_view_traits {
        template<bool Const>
        using iterator = reverse_map_pair_view_iterator<Const, Rng>;

        template<bool Const>
        using sentinel = detail::sentinel_base<reverse_map_pair_view_iterator<Const, Rng>, Const, reverse_map_pair_view<Rng>, Rng>;
    };

    template<bool Const, typename Cls, typename T, typename Rng>
    class member_view_iterator : public detail::iterator_base<member_view_iterator<Const, Cls, T, Rng>, Const, member_view<Rng, Cls, T>, Rng> {
        using _base_t          = detail::iterator_base<member_view_iterator<Const, Cls, T, Rng>, Const, member_view<Rng, Cls, T>, Rng>;
        using _const_base_t    = detail::iterator_base<member_view_iterator<Const, Cls, T, Rng>, _const, member_view<Rng, Cls, T>, Rng>;
        using parent_range     = _base_t::parent_range;
        using adapted_iterator = _base_t::adapted_iterator;

        using traits            = _base_t::traits;
        using adapted_reference = typename traits::iterator_reference;

    public:
        using reference = decltype((std::declval<adapted_reference>()).*(static_cast<T Cls::*>(nullptr)));
        using value_type = std::remove_reference_t<reference>;
        // difference_type and (optionally) iterator_category provided by iterator_base

        constexpr member_view_iterator() = default;

        constexpr member_view_iterator(T Cls::* ptr)
            noexcept(std::is_nothrow_default_constructible_v<_base_t>)
            : _base_t()
            , _ptr(ptr)
        {
            
        }

        constexpr member_view_iterator(parent_range& parent, adapted_iterator current, T Cls::* ptr)
            noexcept(std::is_nothrow_move_constructible_v<adapted_iterator> && std::is_nothrow_constructible_v<_base_t, parent_range&, adapted_iterator>)
            : _base_t(parent, std::move(current))
            , _ptr(ptr)
        {
            
        }

        constexpr member_view_iterator(member_view_iterator<!Const, Cls, T, Rng> it, T Cls::* ptr)
            noexcept(std::is_nothrow_constructible_v<_const_base_t, _base_t>)
            requires Const
            : _base_t(std::move(it))
            , _ptr(ptr)
        {

        }

        [[nodiscard]] constexpr decltype(auto) operator*() const
            noexcept(noexcept(_base_t::operator*()))
        {
            return (_base_t::operator*()).*_ptr;
        }

    private:
        T Cls::* _ptr;
    };

    template<typename Rng, typename Cls, typename T>
    struct member_view_traits {
        template<bool Const>
        using iterator = member_view_iterator<Const, Cls, T, Rng>;

        template<bool Const>
        using sentinel = detail::sentinel_base<member_view_iterator<Const, Cls, T, Rng>, Const, member_view<Rng, Cls, T>, Rng>;
    };

} // namespace detail

template<std::ranges::input_range Rng> requires std::ranges::view<Rng>
class dereference_view :
    public detail::view_impl<
        Rng,
        dereference_view<Rng>,
        detail::dereference_view_traits<Rng>::template iterator,
        detail::dereference_view_traits<Rng>::template sentinel
    >
{
    using _base_t = detail::view_impl<Rng, dereference_view<Rng>, detail::dereference_view_traits<Rng>::template iterator, detail::dereference_view_traits<Rng>::template sentinel>;

public:
    using _base_t::_base_t; // dereference_view(), dereference_view(Rng)
};

template <typename Rng>
dereference_view(Rng&&) -> dereference_view<std::views::all_t<Rng>>;

template<std::ranges::input_range Rng> requires std::ranges::view<Rng>
class reverse_pair_view :
    public detail::view_impl<
        Rng,
        reverse_pair_view<Rng>,
        detail::reverse_pair_view_traits<Rng>::template iterator,
        detail::reverse_pair_view_traits<Rng>::template sentinel
    >
{
    using _base_t = detail::view_impl<Rng, reverse_pair_view<Rng>, detail::reverse_pair_view_traits<Rng>::template iterator, detail::reverse_pair_view_traits<Rng>::template sentinel>;

public:
    using _base_t::_base_t; // reverse_pair_view(), reverse_pair_view(Rng)
};

template <typename Rng>
reverse_pair_view(Rng&&) -> reverse_pair_view<std::views::all_t<Rng>>;

template<std::ranges::input_range Rng> requires std::ranges::view<Rng>
class reverse_map_pair_view :
    public detail::view_impl<
        Rng,
        reverse_map_pair_view<Rng>,
        detail::reverse_map_pair_view_traits<Rng>::template iterator,
        detail::reverse_map_pair_view_traits<Rng>::template sentinel
    >
{
    using _base_t = detail::view_impl<Rng, reverse_map_pair_view<Rng>, detail::reverse_map_pair_view_traits<Rng>::template iterator, detail::reverse_map_pair_view_traits<Rng>::template sentinel>;

public:
    using _base_t::_base_t; // reverse_map_pair_view(), reverse_map_pair_view(Rng)
};

template <typename Rng>
reverse_map_pair_view(Rng&&) -> reverse_map_pair_view<std::views::all_t<Rng>>;

template<std::ranges::input_range Rng, typename Cls, typename T> requires std::ranges::view<Rng>
class member_view :
    public detail::view_impl<
        Rng,
        member_view<Rng, Cls, T>,
        detail::member_view_traits<Rng, Cls, T>::template iterator,
        detail::member_view_traits<Rng, Cls, T>::template sentinel
    >
{
    using _base_t = detail::view_impl<Rng, member_view<Rng, Cls, T>, detail::member_view_traits<Rng, Cls, T>::template iterator, detail::member_view_traits<Rng, Cls, T>::template sentinel>;

    template<bool Const>
    using adapted_iterator = typename _base_t::template adapted_iterator<Const>;

    template<bool Const>
    using traits = typename _base_t::template traits<Const>;

    template<bool Const>
    using iterator = detail::member_view_traits<Rng, Cls, T>::template iterator<Const>;

    template<bool Const>
    using sentinel = detail::member_view_traits<Rng, Cls, T>::template sentinel<Const>;

public:
    member_view(Rng range, T Cls::* ptr) :
        _base_t(std::move(range)),
        _ptr(ptr)
    {

    }

    iterator<detail::_not_const> begin()
        noexcept(noexcept(std::ranges::begin(this->_range)) && std::is_nothrow_move_constructible_v<adapted_iterator<false>>)
    {
        return iterator<false>(*this, std::begin(this->_range), _ptr);
    }

    iterator<detail::_const> begin() const
        noexcept(noexcept(std::ranges::begin(this->_range)) && std::is_nothrow_move_constructible_v<adapted_iterator<true>>)
    {
        return iterator<true>(*this, std::begin(this->_range), _ptr);
    }

    [[nodiscard]] constexpr auto end() noexcept(
        noexcept(std::ranges::end(this->_range)) && std::is_nothrow_move_constructible_v<decltype(std::ranges::end(this->_range))>)
    {
        if constexpr (traits<detail::_not_const>::common) {
            return iterator<detail::_not_const>{*this, std::ranges::end(this->_range), _ptr};
        } else {
            return sentinel<detail::_not_const>{std::ranges::end(this->_range)};
        }
    }

    [[nodiscard]] constexpr auto end() const
        noexcept(noexcept(std::ranges::end(this->_range)) && std::is_nothrow_move_constructible_v<decltype(std::ranges::end(this->_range))>)
        requires std::ranges::range<const Rng>
    {
        if constexpr (traits<detail::_const>::common) {
            return iterator<detail::_const>{*this, std::ranges::end(this->_range), _ptr};
        } else {
            return sentinel<detail::_const>{std::ranges::end(this->_range)};
        }
    }

private:
    T Cls::* _ptr;
};

template <typename Rng, typename Cls, typename T>
member_view(Rng&&, T Cls::*) -> member_view<std::views::all_t<Rng>, Cls, T>;

namespace views {

    namespace detail {

        struct dereference_cpo : public std::ranges::range_adaptor_closure<dereference_cpo> {
            template <std::ranges::viewable_range Rng>
            [[nodiscard]] constexpr auto operator()(Rng&& range) const
                noexcept(noexcept(dereference_view(std::forward<Rng>(range))))
                requires requires { dereference_view(static_cast<Rng&&>(range)); }
            {
                return dereference_view(std::forward<Rng>(range));
            }
        };

        struct reverse_pair_cpo : public std::ranges::range_adaptor_closure<reverse_pair_cpo> {
            template <std::ranges::viewable_range Rng>
            [[nodiscard]] constexpr auto operator()(Rng&& range) const
                noexcept(noexcept(reverse_pair_view(std::forward<Rng>(range))))
                requires requires { reverse_pair_view(static_cast<Rng&&>(range)); }
            {
                return reverse_pair_view(std::forward<Rng>(range));
            }
        };

        struct reverse_map_pair_cpo : public std::ranges::range_adaptor_closure<reverse_map_pair_cpo> {
            template <std::ranges::viewable_range Rng>
            [[nodiscard]] constexpr auto operator()(Rng&& range) const
                noexcept(noexcept(reverse_map_pair_view(std::forward<Rng>(range))))
                requires requires { reverse_map_pair_view(static_cast<Rng&&>(range)); }
            {
                return reverse_map_pair_view(std::forward<Rng>(range));
            }
        };

        struct member_cpo {
            template <std::ranges::viewable_range Rng, typename Cls, typename T>
            [[nodiscard]] constexpr auto operator()(Rng&& range, T Cls::* ptr) const
                noexcept(noexcept(member_view(std::forward<Rng>(range), ptr)))
                requires requires { member_view(static_cast<Rng&&>(range), ptr); }
            {
                return member_view(std::forward<Rng>(range), ptr);
            }

            template <typename Cls, typename T>
            [[nodiscard]] constexpr auto operator()(T Cls::* ptr) const
            {
                return ::tools::ranges::detail::closure<member_cpo, T Cls::*>(ptr);
            }
        };

    }

    inline constexpr detail::dereference_cpo dereference;
    inline constexpr detail::reverse_pair_cpo reverse_pair;
    inline constexpr detail::reverse_map_pair_cpo reverse_map_pair;
    inline constexpr detail::member_cpo member;

} // namespace views

} // namespace tools::ranges

namespace tools {

namespace views = ranges::views; // NOLINT

} // namespace tools

namespace std::ranges {

template <typename Rng>
inline constexpr bool enable_borrowed_range<tools::ranges::dereference_view<Rng>> = enable_borrowed_range<Rng>;

template <typename Rng>
inline constexpr bool enable_borrowed_range<tools::ranges::reverse_pair_view<Rng>> = enable_borrowed_range<Rng>;

template <typename Rng>
inline constexpr bool enable_borrowed_range<tools::ranges::reverse_map_pair_view<Rng>> = enable_borrowed_range<Rng>;

template <typename Rng, typename Cls, typename T>
inline constexpr bool enable_borrowed_range<tools::ranges::member_view<Rng, Cls, T>> = enable_borrowed_range<Rng>;

} // namespace std::ranges

#endif//CPPTOOLS_UTILITY_RANGES_HPP