#ifndef CPPTOOLS__UTILITY__TYPE_TRAITS_IMPL__DETAIL_HPP
#define CPPTOOLS__UTILITY__TYPE_TRAITS_IMPL__DETAIL_HPP

#include <type_traits>

namespace type_utils
{

namespace detail
{

struct sfinae_base
{
    using yes = char;
    using no  = yes[2];
};

template <typename T>
struct has_const_iterator : private sfinae_base
{
private:
    template <typename C> static yes & test(typename C::const_iterator*);
    template <typename C> static no  & test(...);
public:
    static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
    using type =  T;

    void dummy(); //for GCC to supress -Wctor-dtor-privacy
};

template <typename T>
struct has_begin_end : private sfinae_base
{
private:
    template <typename C>
    static yes & f(typename std::enable_if<
        std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::begin)),
                        typename C::const_iterator(C::*)() const>::value>::type*);

    template <typename C> static no & f(...);

    template <typename C>
    static yes & g(typename std::enable_if<
        std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::end)),
                        typename C::const_iterator(C::*)() const>::value, void>::type*);

    template <typename C> static no & g(...);

public:
    static bool const beg_value = sizeof(f<T>(nullptr)) == sizeof(yes);
    static bool const end_value = sizeof(g<T>(nullptr)) == sizeof(yes);

    void dummy(); //for GCC to supress -Wctor-dtor-privacy
};

template <typename T>
struct has_size : private sfinae_base
{
private:
    template <typename C>
    static yes & f(typename std::enable_if<
        std::is_same<decltype(static_cast<typename C::size_type(C::*)() const>(&C::size)),
                        typename C::size_type(C::*)() const>::value>::type*);

    template <typename C> static no & f(...);

public:
    static bool const value = sizeof(f<T>(nullptr)) == sizeof(yes);

    void dummy(); //for GCC to supress -Wctor-dtor-privacy
};

}

} // namespace type_utils

#endif//CPPTOOLS__UTILITY__TYPE_TRAITS_IMPL__DETAIL_HPP