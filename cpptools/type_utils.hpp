#ifndef CPUEMU__COMMON__TYPE_UTILS_HPP
#define CPUEMU__COMMON__TYPE_UTILS_HPP

#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <valarray>

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

template <typename T>
struct is_container : public std::integral_constant<bool,
                                                    detail::has_const_iterator<T>::value &&
                                                    detail::has_begin_end<T>::beg_value  &&
                                                    detail::has_begin_end<T>::end_value> { };

template <typename T, std::size_t N>
struct is_container<T[N]> : std::true_type { };

template <std::size_t N>
struct is_container<char[N]> : std::false_type { };

template <typename T>
struct is_container<std::valarray<T>> : std::true_type { };

template <typename T1, typename T2>
struct is_container<std::pair<T1, T2>> : std::true_type { };

template <typename ...Args>
struct is_container<std::tuple<Args...>> : std::true_type { };

template <typename T>
struct is_sized_container : public std::integral_constant<bool,
                                                    detail::has_const_iterator<T>::value &&
                                                    detail::has_begin_end<T>::beg_value  &&
                                                    detail::has_begin_end<T>::end_value  &&
                                                    detail::has_size<T>::value> { };

template <typename T, std::size_t N>
struct is_sized_container<T[N]> : std::true_type { };

template <std::size_t N>
struct is_sized_container<char[N]> : std::false_type { };

template <typename T>
struct is_sized_container<std::valarray<T>> : std::true_type { };

template <typename T1, typename T2>
struct is_sized_container<std::pair<T1, T2>> : std::true_type { };

template <typename ...Args>
struct is_sized_container<std::tuple<Args...>> : std::true_type { };

namespace
{

std::map<std::string, std::pair<size_t, size_t>> _instanceCount;

}

template <typename T, char* Name>
struct instance_counter
{
    static std::string name = Name;

    instance_counter()
    {
        _instanceCount[name].first++;
        _instanceCount[name].second++;
    }
    
    instance_counter(const instance_counter&)
    {
        _instanceCount[name].first++;
        _instanceCount[name].second++;
    }
protected:
    virtual ~instance_counter() // objects should never be removed through pointers of this type
    {
        _instanceCount[name].second++;
    }
};

void print_instance_count(std::ostream& out = std::cout);

}

#endif//CPUEMU__COMMON__TYPE_UTILS_HPP