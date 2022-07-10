#ifndef TOOLS__UTILITY__TYPE_IMPL__DINSTANCE_COUNTERHPP
#define TOOLS__UTILITY__TYPE_IMPL__DINSTANCE_COUNTERHPP

#include <iostream>
#include <map>
#include <string>
#include <type_traits>

namespace tools::type
{

namespace
{

std::map<std::string, std::pair<size_t, size_t>> _instance_count;

}

template <typename T, const char* Name>
struct instance_counter
{
    static inline const char* name = Name;

    instance_counter()
    {
        _instance_count[std::string(name)].first++;
        _instance_count[std::string(name)].second++;
    }
    
    instance_counter(const instance_counter&)
    {
        _instance_count[std::string(name)].first++;
        _instance_count[std::string(name)].second++;
    }
protected:
    virtual ~instance_counter() // objects should never be removed through pointers of this type
    {
        _instance_count[std::string(name)].second--;
    }
};

void print_instance_count(std::ostream& out = std::cout);

} // namespace tools::type

#endif//TOOLS__UTILITY__TYPE_IMPL__INSTANCE_COUNTER_HPP