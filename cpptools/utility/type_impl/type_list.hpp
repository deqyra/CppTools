#ifndef TOOLS__UTILITY__TYPE_IMPL__TYPE_LIST_HPP
#define TOOLS__UTILITY__TYPE_IMPL__TYPE_LIST_HPP

namespace tools::type
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

// Specialisation combining 2 type_lists
template<typename... Head, typename... Tail>
struct make_type_list<
    type_list<Head...>,
    type_list<Tail...>
>
{
    using type = type_list<Head..., Tail...>;
};

// Specialisation combining a type_list with a list of the rest
template<typename... Head, typename... Tail>
struct make_type_list<type_list<Head...>, Tail...>
{
    using type = type_list<Head..., Tail...>;
};

// Specialisation combining a regular type with a list of the rest
template<typename Head, typename... Tail>
struct make_type_list<Head, Tail...>
{
    using type = type_list<Head, Tail...>;
};

} // namespace tools::type

#endif//TOOLS__UTILITY__TYPE_IMPL__TYPE_LIST_HPP
