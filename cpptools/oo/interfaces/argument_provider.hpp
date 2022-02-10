#ifndef CPPTOOLS__OO__INTERFACES__ARGUMENT_PROVIDER_HPP
#define CPPTOOLS__OO__INTERFACES__ARGUMENT_PROVIDER_HPP

#include <utility>

#include "../../utility/type_traits.hpp"

namespace cpptools
{

template<typename... Ts>
class ArgumentProvider
{
public:
    using ArgTypes = type_utils::type_list<Ts...>;
    using ArgTuple = std::tuple<Ts...>;
    static constexpr size_t ArgTupleSize = std::tuple_size_v<ArgTuple>;

    virtual ArgTuple getArguments() = 0;
};

} // namespace cpptools

#endif//CPPTOOLS__OO__INTERFACES__ARGUMENT_PROVIDER_HPP