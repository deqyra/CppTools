#include "type_traits.hpp"

namespace type_utils
{

void print_instance_count(std::ostream& out)
{
    for (const auto& [k, counts] : _instanceCount)
    {
        out << "Type \"" << k << "\":\n"
            << counts.first << " instances created, " << counts.second << " alive\n";
    }
    out << std::endl;
}

}