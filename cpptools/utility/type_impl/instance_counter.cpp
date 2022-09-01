#include "instance_counter.hpp"

namespace tools::type
{

void print_instance_count(std::ostream& out)
{
    for (const auto& [k, counts] : _instance_count)
    {
        out << "Type \"" << k << "\":\n"
            << counts.first << " instances created, " << counts.second << " alive\n";
    }
    out << std::endl;
}

} // namespace tools::type
