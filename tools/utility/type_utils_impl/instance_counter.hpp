#ifndef TOOLS__UTILITY__TYPE_TRAITS_IMPL__DINSTANCE_COUNTERHPP
#define TOOLS__UTILITY__TYPE_TRAITS_IMPL__DINSTANCE_COUNTERHPP

#include <iostream>
#include <map>
#include <string>
#include <type_traits>

namespace type_utils
{

namespace
{

std::map<std::string, std::pair<size_t, size_t>> _instanceCount;

}

template <typename T, char* Name>
struct instance_counter
{
    static inline const char* name = Name;

    instance_counter()
    {
        _instanceCount[std::string(name)].first++;
        _instanceCount[std::string(name)].second++;
    }
    
    instance_counter(const instance_counter&)
    {
        _instanceCount[std::string(name)].first++;
        _instanceCount[std::string(name)].second++;
    }
protected:
    virtual ~instance_counter() // objects should never be removed through pointers of this type
    {
        _instanceCount[std::string(name)].second--;
    }
};

void print_instance_count(std::ostream& out = std::cout);


} // namespace type_utils

#endif//TOOLS__UTILITY__TYPE_TRAITS_IMPL__INSTANCE_COUNTER_HPP