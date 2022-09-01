#ifndef CPPTOOLS__UTILITY__TYPE_TRAITS_IMPL__TYPE_LIST_HPP
#define CPPTOOLS__UTILITY__TYPE_TRAITS_IMPL__TYPE_LIST_HPP

namespace cpptools::type_utils
{

/////////////////////////
///     type_list     ///
/////////////////////////

template <typename...>
struct type_list {};

///////////////////////////
///     apply_types     ///
///////////////////////////

template <template <typename...> typename TT, typename L>
struct apply_types;

template <
    template <typename...> typename TT,
    typename... Ts>
struct apply_types<TT, type_list<Ts...>>
{
    using type = TT<Ts...>;
};

//////////////////////////////
///     make_type_list     ///
//////////////////////////////

// Base
template<typename...>
struct make_type_list
{
    using type = type_list<>;
};

// Specialization for empty case
template<>
struct make_type_list<>
{
    using type = type_list<>;
};

// Specialisation to end recursion on a type list
template<typename... AllTypes>
struct make_type_list<type_list<AllTypes...>>
{
    using type = type_list<AllTypes...>;
};

// Specialisation to end recursion on a regular type
template<typename T>
struct make_type_list<T>
{
    using type = type_list<T>;
};

// Specialisation for combining 2 type_lists
template<typename... Head, typename... Tail>
struct make_type_list<type_list<Head...>, type_list<Tail...>>
{
    using type = type_list<Head..., Tail...>;
};

// Specialisation for combining a first type with whatever comes next
template<typename Head, typename... Tail>
struct make_type_list<Head, Tail...>
{
    using type = typename make_type_list<
        typename make_type_list<Head>::type,
        typename make_type_list<Tail...>::type
    >::type;
};

} // namespace cpptools::type_utils

#endif//CPPTOOLS__UTILITY__TYPE_TRAITS_IMPL__TYPE_LIST_HPP
